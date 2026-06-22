package config

import (
	"crypto/subtle"
	"flag"
	"os"
	"time"
)

type Permission uint8

var (
	PERMISSION_NONE  Permission = 0
	PERMISSION_READ  Permission = 1 << 0
	PERMISSION_WRITE Permission = 1 << 1
)

type Config struct {
	ConnectAddress *string
	ServerAddress  *string
	SizeLimit      *int64

	ContextTimeout  *time.Duration
	GracefulTimeout *time.Duration

	ServerToken *[]byte
	ServerPerms Permission
}

func (c *Config) Collect() {
	c.ConnectAddress = flag.String("connect", "localhost:55543", "Address to connect TCP client")
	c.ServerAddress = flag.String("serve", "localhost:5543", "Address to serve HTTP server")
	c.SizeLimit = flag.Int64("size-limit", 1024*1024, "Maximum accepted size of the request body")

	c.ContextTimeout = flag.Duration("context-timeout", 5*time.Second, "Context timeout for requests")
	c.GracefulTimeout = flag.Duration("graceful-timeout", 5*time.Second, "Graceful shutdown timeout for server")

	flag.Parse()

	token, exists := os.LookupEnv("LETDD_TOKEN")
	if !exists {
		c.ServerToken = nil
	} else {
		tokenBytes := []byte(token)
		c.ServerToken = &tokenBytes
	}

	switch os.Getenv("LETDD_PERMISSIONS") {
	case "read":
		c.ServerPerms = PERMISSION_READ
	case "write":
		c.ServerPerms = PERMISSION_WRITE
	case "read,write", "write,read":
		c.ServerPerms = (PERMISSION_READ | PERMISSION_WRITE)
	default:
		c.ServerPerms = PERMISSION_NONE
	}
}

func (c *Config) IsMatches(token string) bool {
	if c.ServerToken == nil {
		return false
	}

	tokenBytes := []byte(token)
	if len(*c.ServerToken) != len(tokenBytes) {
		return false
	}

	return subtle.ConstantTimeCompare(*c.ServerToken, tokenBytes) == 1
}

func (c *Config) IsPermitted(permission Permission) bool {
	return c.ServerPerms&permission != 0
}
