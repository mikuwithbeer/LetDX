package http

import (
	"LetDD/config"

	"github.com/labstack/echo/v5"
	"github.com/labstack/echo/v5/middleware"
)

func (s *Server) Authorization(ctx *echo.Context, userToken string, _ middleware.ExtractorSource) (bool, error) {
	if !s.Config.IsMatches(userToken) {
		return false, nil
	}

	switch ctx.Request().Method {
	case "GET":
		if !s.Config.IsPermitted(config.PERMISSION_READ) {
			return false, nil
		}
	case "POST", "PUT":
		if !s.Config.IsPermitted(config.PERMISSION_WRITE) {
			return false, nil
		}
	}

	return true, nil
}
