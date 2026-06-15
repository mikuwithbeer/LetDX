package tcp

import (
	"bufio"
	"fmt"
	"net"
	"sync"
)

type Client struct {
	connection   net.Conn
	transactions uint64

	reader *bufio.Reader
	writer *bufio.Writer

	mutex sync.Mutex
}

func NewClient(address string) (*Client, error) {
	connection, err := net.Dial("tcp", address)
	if err != nil {
		return nil, err
	}

	client := &Client{
		connection:   connection,
		transactions: 0,

		reader: bufio.NewReader(connection),
		writer: bufio.NewWriter(connection),
	}

	response, err := client.Communicate(CountEntriesRequest{})
	if err != nil {
		return nil, err
	}

	if response.Kind() == CountEntriesResponseKind {
		client.transactions = response.(CountEntriesResponse).Count
	} else {
		return nil, fmt.Errorf("unexpected response: %s", response.Kind())
	}

	return client, nil
}

func (c *Client) WalID() uint64 {
	return c.transactions
}

func (c *Client) Communicate(request Request) (Response, error) {
	c.mutex.Lock()
	defer c.mutex.Unlock()

	updateTransactions := false
	switch request.(type) {
	case AddAccountRequest, MakeTransferRequest, UpdateAccountRequest:
		updateTransactions = true
	}

	if err := c.sendRequest(request); err != nil {
		return nil, err
	}

	response, err := c.receiveResponse()
	if err != nil {
		return nil, err
	}

	if updateTransactions && response.Kind() != ErrorResponseKind {
		c.transactions++
	}

	return response, nil
}

func (c *Client) Close() error {
	if c.connection == nil {
		return nil
	}

	return c.connection.Close()
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
