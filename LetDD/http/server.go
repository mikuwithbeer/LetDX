package http

import (
	"LetDD/config"
	"LetDD/tcp"

	"context"

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

	s.Echo.GET("/accounts/:id", s.getAccount)
	s.Echo.PUT("/accounts/:id", s.updateAccount)
	s.Echo.POST("/accounts", s.postAccount)
	s.Echo.POST("/transfers", s.postTransfer)

	sc := echo.StartConfig{Address: *s.Config.ServerAddress, HideBanner: true}
	return sc.Start(ctx, s.Echo)
}
