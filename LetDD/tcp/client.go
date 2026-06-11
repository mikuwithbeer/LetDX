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

	Input  chan Command
	Output chan []byte
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

		Input:  make(chan Command),
		Output: make(chan []byte),
		Errors: make(chan error),
	}

	return client, nil
}

func (c *Client) Start() {
	c.running = true

	go c.readConnection()
	go c.writeConnection()
}

func (c *Client) Send(command Command) {
	c.Input <- command
}

func (c *Client) Receive() []byte {
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
		reply, err := reader.ReadBytes('\n')
		if err != nil {
			if errors.Is(err, net.ErrClosed) || err == io.EOF {
				return
			}

			c.Errors <- err
			return
		}

		c.Output <- reply
	}
}

func (c *Client) writeConnection() {
	for message := range c.Input {
		if !c.running {
			return
		}

		encoded := message.Encode()
		_, err := c.connection.Write(encoded)
		if err != nil {
			c.Errors <- err
			return
		}
	}
}
