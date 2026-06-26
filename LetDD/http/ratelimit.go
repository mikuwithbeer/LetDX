package http

import (
	"net/http"
	"strconv"
	"time"

	"github.com/labstack/echo/v5"
	"github.com/labstack/echo/v5/middleware"
)

// Configures and returns a rate limiter middleware for the server.
func (s *Server) RateLimiter() middleware.RateLimiterConfig {
	return middleware.RateLimiterConfig{
		Skipper: func(c *echo.Context) bool {
			// Skip rate limiting for other endpoints.
			if c.Request().Method == http.MethodPost && c.Request().URL.Path == "/transfers" {
				return false
			}

			return true
		},
		Store: middleware.NewRateLimiterMemoryStoreWithConfig(
			middleware.RateLimiterMemoryStoreConfig{
				Rate:      1.0 / 60.0,
				Burst:     1,
				ExpiresIn: 3 * time.Minute,
			},
		),
		IdentifierExtractor: func(ctx *echo.Context) (string, error) {
			// Extract the transfer request from the context.
			postTransfer, err := BindAndValidate[PostTransferRequest](ctx)
			if err != nil {
				return "", err
			}

			ctx.Set("transfer", postTransfer) // Store the request in the context for later use
			return strconv.FormatUint(*postTransfer.FromID, 10), nil
		},
		ErrorHandler: func(c *echo.Context, err error) error {
			return c.JSON(http.StatusForbidden, map[string]string{"error": "Invalid request"})
		},
		DenyHandler: func(c *echo.Context, identifier string, err error) error {
			return c.JSON(http.StatusTooManyRequests, map[string]string{"error": "Rate limit exceeded"})
		},
	}
}
