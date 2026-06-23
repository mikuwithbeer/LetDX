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

type Client struct {
	connection   net.Conn
	address      string
	transactions atomic.Uint64

	reader *bufio.Reader
	writer *bufio.Writer

	supervisor *supervisor.Supervisor
	mutex      sync.Mutex
}

func NewClient(address string) *Client {
	return &Client{
		connection:   nil,
		address:      address,
		transactions: atomic.Uint64{},

		reader: nil,
		writer: nil,

		supervisor: supervisor.NewSupervisor(5, 200*time.Millisecond, 5*time.Second),
		mutex:      sync.Mutex{},
	}
}

func (c *Client) WalID() uint64 {
	return c.transactions.Load()
}

func (c *Client) Communicate(ctx context.Context, request Request) (Response, error) {
	c.mutex.Lock()
	defer c.mutex.Unlock()

	updateTransactions := false
	switch request.(type) {
	case AddAccountRequest, MakeTransferRequest, UpdateAccountRequest:
		updateTransactions = true
	}

	var response Response
	var err error

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

	c.supervisor.Reset()

	if ctx.Err() != nil {
		return nil, fmt.Errorf("communication aborted: %w", ctx.Err())
	}

	if err != nil {
		return nil, fmt.Errorf("LetDB communication failed after retries: %w", err)
	}

	if updateTransactions && response.Kind() != ErrorResponseKind {
		c.transactions.Add(1)
	}

	return response, nil
}

func (c *Client) Close() error {
	c.mutex.Lock()
	defer c.mutex.Unlock()

	return c.disconnect()
}

func (c *Client) connect(ctx context.Context) error {
	if c.connection != nil {
		return nil
	}

	dialer := net.Dialer{}
	connection, err := dialer.DialContext(ctx, "tcp", c.address)

	if ctx.Err() != nil {
		return fmt.Errorf("connection aborted: %w", ctx.Err())
	}

	if err != nil {
		return err
	}

	c.connection = connection

	c.reader = bufio.NewReader(connection)
	c.writer = bufio.NewWriter(connection)

	if err := c.sendRequest(CountEntriesRequest{}); err != nil {
		c.disconnect()
		return fmt.Errorf("handshake send failed: %w", err)
	}

	response, err := c.receiveResponse()
	if err != nil {
		c.disconnect()
		return fmt.Errorf("handshake receive failed: %w", err)
	}

	if response.Kind() == CountEntriesResponseKind {
		c.transactions.Store(response.(CountEntriesResponse).Count)
	} else {
		c.disconnect()
		return fmt.Errorf("unexpected handshake response: %s", response.Kind())
	}

	return nil
}

func (c *Client) sendRequest(request Request) error {
	_, err := c.writer.Write(request.Encode())
	if err != nil {
		return err
	}

	return c.writer.Flush()
}

func (c *Client) receiveResponse() (Response, error) {
	data, err := c.reader.ReadBytes('\n')
	if err != nil {
		return nil, err
	}

	response, err := ParseResponse(data)
	if err != nil {
		return nil, err
	}

	return response, nil
}

func (c *Client) disconnect() error {
	if c.connection == nil {
		return nil
	}

	err := c.connection.Close()

	c.connection = nil

	c.reader = nil
	c.writer = nil

	if err != nil {
		return fmt.Errorf("failed to close connection: %w", err)
	}

	return nil
}
