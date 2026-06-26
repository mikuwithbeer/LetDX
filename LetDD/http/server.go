package http

import (
	"LetDD/config"
	"LetDD/tcp"

	"context"
	"errors"
	"net/http"

	"github.com/labstack/echo/v5"
	"github.com/labstack/echo/v5/middleware"
)

// Represents the HTTP server.
type Server struct {
	Echo   *echo.Echo
	Client *tcp.Client
	Config *config.Config
}

// Creates a new instance of the `Server`.
func NewServer(client *tcp.Client, config *config.Config) *Server {
	echo := echo.New()
	echo.Validator = NewRequestValidator()

	return &Server{
		Echo:   echo,
		Client: client,
		Config: config,
	}
}

// Starts the HTTP server, setting up routes, middleware, and handling graceful shutdown.
func (s *Server) Start(ctx context.Context) error {
	// Middleware to log incoming requests.
	s.Echo.Use(middleware.RequestLogger())

	// Middleware to limit the size of incoming request.
	s.Echo.Use(middleware.BodyLimit(s.Config.SizeLimit))

	// Middleware to handle authorization based on the provided server token.
	if len(s.Config.ServerToken) > 0 {
		s.Echo.Use(middleware.KeyAuth(s.Authorization))
	}

	// Middleware to limit the rate of incoming requests.
	s.Echo.Use(middleware.RateLimiterWithConfig(s.RateLimiter()))

	// Middleware to set a timeout for the request context.
	s.Echo.Use(middleware.ContextTimeout(s.Config.ContextTimeout))

	// Define the routes and their corresponding handlers.
	s.Echo.GET("/accounts/:id", s.GetAccount)
	s.Echo.PUT("/accounts/:id", s.UpdateAccount)
	s.Echo.POST("/accounts", s.PostAccount)
	s.Echo.POST("/transfers", s.PostTransfer)

	// Start the server and handle any errors that occur during startup.
	sc := echo.StartConfig{
		Address:         s.Config.ServerAddress,
		GracefulTimeout: s.Config.GracefulTimeout,
		HideBanner:      true,
	}

	if err := sc.Start(ctx, s.Echo); err != nil && !errors.Is(err, http.ErrServerClosed) {
		return err
	}

	return nil
}

// Communicates with the TCP client, sending the request and handling the response.
func (s *Server) Communicate(ctx *echo.Context, request tcp.Request) error {
	// Send the request to the TCP client and wait for a response.
	clientResponse, err := s.Client.Communicate(ctx.Request().Context(), request)
	if err != nil {
		return err
	}

	// Convert the TCP response to an HTTP response.
	serverResponse, err := ToResponse(clientResponse)
	if err != nil {
		return err
	}

	return serverResponse.ToJSON(ctx)
}
