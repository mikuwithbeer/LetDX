package http

import (
	"LetDD/config"

	"net/http"

	"github.com/labstack/echo/v5"
	"github.com/labstack/echo/v5/middleware"
)

// Validates the provided user token against the configured server token and checks permissions.
func (s *Server) Authorization(ctx *echo.Context, userToken string, _ middleware.ExtractorSource) (bool, error) {
	if !s.Config.IsMatches(userToken) {
		return false, nil
	}

	switch ctx.Request().Method {
	case http.MethodGet:
		if !s.Config.IsPermitted(config.PERMISSION_READ) {
			return false, nil
		}
	case http.MethodPost, http.MethodPut:
		if !s.Config.IsPermitted(config.PERMISSION_WRITE) {
			return false, nil
		}
	default:
		return false, nil
	}

	return true, nil
}
