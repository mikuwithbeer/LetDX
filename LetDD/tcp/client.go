package tcp

import (
	"LetDD/supervisor"

	"bufio"
	"context"
	"fmt"
	"net"
	"sync"
	"sync/atomic"
	"time"
)

// Represents a TCP client that communicates with a server.
type Client struct {
	connection net.Conn
	address    string

	transactions atomic.Uint64 // Keeps track of the number of transactions

	reader *bufio.Reader
	writer *bufio.Writer

	supervisor *supervisor.Supervisor // Manages retries and backoff for communication attempts
	mutex      sync.Mutex             // Ensures that only one operation is performed at a time
}

// Creates a new TCP client instance with the specified server address.
func NewClient(address string) *Client {
	// The supervisor is configured to try up to 5 times, starting with a 200ms delay up to a maximum of 5 seconds.
	supervisor := supervisor.NewSupervisor(5, 200*time.Millisecond, 5*time.Second)

	return &Client{
		connection:   nil,
		address:      address,
		transactions: atomic.Uint64{},

		reader: nil,
		writer: nil,

		supervisor: supervisor,
		mutex:      sync.Mutex{},
	}
}

// Returns the current transaction count.
func (c *Client) WalID() uint64 {
	return c.transactions.Load()
}

// Communicates with the server by sending a request and receiving a response.
func (c *Client) Communicate(ctx context.Context, request Request) (Response, error) {
	c.mutex.Lock()
	defer c.mutex.Unlock()

	// Determine if the request is one that modifies transactions.
	updateTransactions := false
	switch request.(type) {
	case AddAccountRequest, MakeTransferRequest, UpdateAccountRequest:
		updateTransactions = true
	}

	var response Response
	var err error

	// Attempt to communicate with the server.
	for c.supervisor.Try(ctx) {
		if err = c.connect(ctx); err != nil {
			continue
		}

		if err = c.sendRequest(request); err != nil {
			c.disconnect()
			continue
		}

		if response, err = c.receiveResponse(); err != nil {
			c.disconnect()
			continue
		}

		break
	}

	c.supervisor.Reset() // Prepare for the next communication attempt

	if ctx.Err() != nil {
		return nil, fmt.Errorf("communication aborted: %w", ctx.Err())
	}

	if err != nil {
		return nil, fmt.Errorf("LetDB communication failed after retries: %w", err)
	}

	// Increment the transaction counter.
	if updateTransactions && response.Kind() != ErrorResponseKind {
		c.transactions.Add(1)
	}

	return response, nil
}

// Closes the TCP client connection.
func (c *Client) Close() error {
	c.mutex.Lock()
	defer c.mutex.Unlock()

	return c.disconnect()
}

// Establishes a connection to the server.
func (c *Client) connect(ctx context.Context) error {
	if c.connection != nil {
		return nil
	}

	// Use a dialer to establish a TCP connection with the server.
	dialer := net.Dialer{}
	connection, err := dialer.DialContext(ctx, "tcp", c.address)

	if ctx.Err() != nil {
		return fmt.Errorf("connection aborted: %w", ctx.Err())
	}

	if err != nil {
		return err
	}

	c.connection = connection // Store the established connection

	c.reader = bufio.NewReader(connection) // Initialize a buffered reader for the connection
	c.writer = bufio.NewWriter(connection) // Initialize a buffered writer for the connection

	// Synchronize with the server to get the current transaction count.
	if err := c.sendRequest(CountEntriesRequest{}); err != nil {
		c.disconnect()
		return fmt.Errorf("handshake send failed: %w", err)
	}

	response, err := c.receiveResponse()
	if err != nil {
		c.disconnect()
		return fmt.Errorf("handshake receive failed: %w", err)
	}

	// Handle the response from the server.
	if response.Kind() == CountEntriesResponseKind {
		c.transactions.Store(response.(CountEntriesResponse).Count)
	} else {
		c.disconnect()
		return fmt.Errorf("unexpected handshake response: %s", response.Kind())
	}

	return nil
}

// Sends a request to the server.
func (c *Client) sendRequest(request Request) error {
	if _, err := c.writer.Write(request.Encode()); err != nil {
		return err
	}

	return c.writer.Flush()
}

// Receives a response from the server.
func (c *Client) receiveResponse() (Response, error) {
	data, err := c.reader.ReadBytes('\n')
	if err != nil {
		return nil, err
	}

	return ParseResponse(data)
}

// Disconnects the client from the server.
func (c *Client) disconnect() error {
	if c.connection == nil {
		return nil
	}

	// Close the underlying TCP connection.
	err := c.connection.Close()

	// Clean up the client state.
	c.connection = nil

	c.reader = nil
	c.writer = nil

	if err != nil {
		return fmt.Errorf("failed to close connection: %w", err)
	}

	return nil
}
