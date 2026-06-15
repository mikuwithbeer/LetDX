package http

import (
	"github.com/labstack/echo/v5"
	"github.com/labstack/echo/v5/middleware"
)

type Server struct {
	Echo *echo.Echo

	GetAccount    echo.HandlerFunc
	PostAccount   echo.HandlerFunc
	UpdateAccount echo.HandlerFunc
	PostTransfer  echo.HandlerFunc
}

func NewServer() *Server {
	return &Server{
		Echo: echo.New(),
	}
}

func (s *Server) Start(address string) error {
	s.Echo.Use(middleware.RequestLogger())

	s.Echo.GET("/account", s.GetAccount)
	s.Echo.POST("/account", s.PostAccount)
	s.Echo.PUT("/account", s.UpdateAccount)
	s.Echo.POST("/transfer", s.PostTransfer)

	return s.Echo.Start(address)
}
