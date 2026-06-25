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

type Server struct {
	Echo   *echo.Echo
	Client *tcp.Client
	Config *config.Config
}

func NewServer(client *tcp.Client, config *config.Config) *Server {
	echo := echo.New()
	echo.Validator = NewRequestValidator()

	return &Server{
		Echo:   echo,
		Client: client,
		Config: config,
	}
}

func (s *Server) Start(ctx context.Context) error {
	if s.Config.ServerToken != nil {
		s.Echo.Use(middleware.KeyAuth(s.Authorization))
	}

	s.Echo.Use(middleware.RequestLogger())

	s.Echo.Use(middleware.BodyLimit(*s.Config.SizeLimit))
	s.Echo.Use(middleware.ContextTimeout(*s.Config.ContextTimeout))

	s.Echo.GET("/accounts/:id", s.getAccount)
	s.Echo.PUT("/accounts/:id", s.updateAccount)
	s.Echo.POST("/accounts", s.postAccount)
	s.Echo.POST("/transfers", s.postTransfer)

	sc := echo.StartConfig{
		Address:         *s.Config.ServerAddress,
		GracefulTimeout: *s.Config.GracefulTimeout,
		HideBanner:      true,
	}

	if err := sc.Start(ctx, s.Echo); err != nil && !errors.Is(err, http.ErrServerClosed) {
		return err
	}

	return nil
}
