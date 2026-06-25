package config

import (
	"crypto/subtle"
	"flag"
	"os"
	"strings"
	"time"
)

// Represents the permission level for the server.
// It is defined as a bitmask, allowing for combinations of permissions.
type Permission uint8

const (
	PERMISSION_NONE  Permission = 0      // No permissions
	PERMISSION_READ  Permission = 1 << 0 // Read permission (GET)
	PERMISSION_WRITE Permission = 1 << 1 // Write permission (POST, PUT)
)

// Holds all the operational parameters required to run the server and establish client connections.
type Config struct {
	ConnectAddress string
	ServerAddress  string
	SizeLimit      int64

	ContextTimeout  time.Duration
	GracefulTimeout time.Duration

	ServerToken []byte
	ServerPerms Permission
}

// Collects configuration settings from command-line flags and environment variables.
func (c *Config) Collect() {
	// Define command-line flags.
	flag.StringVar(&c.ConnectAddress, "connect", "localhost:55543", "Address to connect TCP client")
	flag.StringVar(&c.ServerAddress, "serve", "localhost:5543", "Address to serve HTTP server")
	flag.Int64Var(&c.SizeLimit, "size-limit", 1024*1024, "Maximum accepted size of the request body")

	flag.DurationVar(&c.ContextTimeout, "context-timeout", 5*time.Second, "Context timeout for requests")
	flag.DurationVar(&c.GracefulTimeout, "graceful-timeout", 5*time.Second, "Graceful shutdown timeout for server")

	flag.Parse() // Parse command-line flags

	// Read server token from environment variable.
	if token, exists := os.LookupEnv("LETDD_TOKEN"); exists && token != "" {
		c.ServerToken = []byte(token)
	} else {
		c.ServerToken = nil
	}

	// Read server permissions from environment variable.
	permissions := strings.ToLower(os.Getenv("LETDD_PERMISSIONS"))
	switch permissions {
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

// Checks if the provided token matches the configured token.
// Uses constant-time comparison for security.
func (c *Config) IsMatches(token string) bool {
	if len(c.ServerToken) == 0 {
		return false
	}

	tokenBytes := []byte(token)
	if len(c.ServerToken) != len(tokenBytes) {
		return false
	}

	return subtle.ConstantTimeCompare(c.ServerToken, tokenBytes) == 1 // Constant-time comparison
}

// Checks if the specified permission is granted.
func (c *Config) IsPermitted(permission Permission) bool {
	return c.ServerPerms&permission != 0
}
