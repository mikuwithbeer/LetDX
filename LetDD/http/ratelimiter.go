package http

import (
	"net/http"
	"strconv"

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
		IdentifierExtractor: func(ctx *echo.Context) (string, error) {
			// Extract the transfer request from the context.
			postTransfer, err := BindAndValidate[PostTransferRequest](ctx)
			if err != nil {
				return "", err
			}

			ctx.Set("transfer", postTransfer) // Store the request in the context for later use
			return strconv.FormatUint(*postTransfer.FromID, 10), nil
		},
		Store: middleware.NewRateLimiterMemoryStoreWithConfig(*s.Config.RateLimiter), // Guaranteed that rate limiter store is not nil here
		ErrorHandler: func(c *echo.Context, err error) error {
			return c.JSON(http.StatusBadRequest, map[string]string{"message": "Invalid Request"})
		},
		DenyHandler: func(c *echo.Context, identifier string, err error) error {
			return c.JSON(http.StatusTooManyRequests, map[string]string{"message": "Need to Cooldown"})
		},
	}
}
