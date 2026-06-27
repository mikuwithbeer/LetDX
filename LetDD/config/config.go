package config

import (
	"LetDD/supervisor"

	"crypto/subtle"
	"flag"
	"os"
	"strings"
	"time"

	"github.com/labstack/echo/v5/middleware"
)

// Represents the permission level for the server.
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

	ContextTimeout  time.Duration
	GracefulTimeout time.Duration

	BodyLimit int64

	RateLimiter *middleware.RateLimiterMemoryStoreConfig
	Supervisor  *supervisor.Supervisor

	ServerToken []byte
	ServerPerms Permission
}

// Collects configuration settings from command-line flags and environment variables.
func (c *Config) Collect() error {
	// Define command-line flags.
	flag.StringVar(&c.ConnectAddress, "connect", "localhost:55543", "Address to connect TCP client")
	flag.StringVar(&c.ServerAddress, "serve", "localhost:5543", "Address to serve HTTP server")

	flag.DurationVar(&c.ContextTimeout, "context-timeout", 5*time.Second, "Context timeout for requests")
	flag.DurationVar(&c.GracefulTimeout, "graceful-timeout", 5*time.Second, "Graceful shutdown timeout for server")

	flag.Int64Var(&c.BodyLimit, "limit", 1024*1024, "Maximum accepted size of the request body")

	ratelimitString := flag.String("ratelimit", "", "Rate limit for transfers in the format <requests>/<window>[/<burst>][/<expires>]")
	supervisorString := flag.String("supervisor", "", "Supervisor configuration in the format <attempt>/<maximum>/<minimum>")

	flag.Parse() // Parse command-line flags

	// Parse rate limiter configuration.
	if ratelimitString != nil && *ratelimitString != "" {
		ratelimiter, err := ParseRateLimiter(*ratelimitString)
		if err != nil {
			return err
		}

		c.RateLimiter = &ratelimiter
	}

	// Parse supervisor configuration.
	if supervisorString != nil && *supervisorString != "" {
		observer, err := ParseSupervisor(*supervisorString)
		if err != nil {
			return err
		}

		c.Supervisor = &observer
	}

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

	return nil
}

// Checks if the provided token matches the configured token.
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
