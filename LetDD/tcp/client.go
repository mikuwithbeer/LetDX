package tcp

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"net"
)

type Client struct {
	connection net.Conn
	running    bool

	Input  chan Request
	Output chan Response
	Errors chan error
}

func NewClient(port uint16) (*Client, error) {
	adress := fmt.Sprintf("localhost:%d", port)
	connection, err := net.Dial("tcp", adress)
	if err != nil {
		return nil, err
	}

	client := &Client{
		connection: connection,
		running:    false,

		Input:  make(chan Request),
		Output: make(chan Response),
		Errors: make(chan error),
	}

	return client, nil
}

func (c *Client) Start() {
	c.running = true

	go c.readConnection()
	go c.writeConnection()
}

func (c *Client) Send(request Request) {
	c.Input <- request
}

func (c *Client) Receive() Response {
	return <-c.Output
}

func (c *Client) Close() {
	c.connection.Close()
	c.running = false

	close(c.Input)
	close(c.Output)
	close(c.Errors)
}

func (c *Client) readConnection() {
	reader := bufio.NewReader(c.connection)

	for c.running {
		response, err := reader.ReadBytes('\n')
		if err != nil {
			if errors.Is(err, net.ErrClosed) || err == io.EOF {
				c.running = false
				return
			}

			c.Errors <- err
		} else {
			parsedResponse, err := ParseResponse(response)
			if err != nil {
				c.Errors <- err
			} else {
				c.Output <- parsedResponse
			}
		}
	}
}

func (c *Client) writeConnection() {
	writer := bufio.NewWriter(c.connection)

	for request := range c.Input {
		if !c.running {
			return
		}

		encoded := request.Encode()
		_, err := writer.Write(encoded)
		if err != nil {
			c.Errors <- err
		}

		writer.Flush()
	}
}
