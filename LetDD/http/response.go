package http

import (
	"LetDD/tcp"
	"LetDD/uint128"

	"fmt"
	"net/http"

	"github.com/labstack/echo/v5"
)

// Defines the interface for server responses.
type Response interface {
	// Converts the response to JSON and writes it to the provided context.
	ToJSON(ctx *echo.Context) error
}

// Represents the structure of a response for retrieving account details.
type GetAccountResponse struct {
	Credits uint128.Uint128 `json:"credits"`
	Debits  uint128.Uint128 `json:"debits"`
	Flags   Flags           `json:"flags"`
}

func (r GetAccountResponse) ToJSON(ctx *echo.Context) error {
	return ctx.JSON(http.StatusOK, r)
}

// Represents the structure of a response for creating a new account.
type PostAccountResponse struct {
	AccountID uint64 `json:"id"`
}

func (r PostAccountResponse) ToJSON(ctx *echo.Context) error {
	return ctx.JSON(http.StatusCreated, r)
}

// Represents the structure of a response for a successful operation.
type OkResponse struct{}

func (r OkResponse) ToJSON(ctx *echo.Context) error {
	return ctx.NoContent(http.StatusNoContent)
}

// Represents the structure of a response for an error.
type ErrorResponse struct {
	Message string `json:"message"` // Meant to be compatible with the Echo's error handling
	Code    uint16 `json:"code"`    // Provides a specific error code for the client
}

func (r ErrorResponse) ToJSON(ctx *echo.Context) error {
	return ctx.JSON(http.StatusBadRequest, r)
}

// Converts a TCP response to an HTTP response.
func ToResponse(r tcp.Response) (Response, error) {
	switch response := r.(type) {
	case tcp.GetAccountResponse:
		return GetAccountResponse{Credits: response.Credits,
			Debits: response.Debits,
			Flags:  Uint8ToFlags(response.Flags),
		}, nil
	case tcp.AddAccountResponse:
		return PostAccountResponse{
			AccountID: response.AccountID,
		}, nil
	case tcp.OkResponse:
		return OkResponse{}, nil
	case tcp.ErrorResponse:
		return ErrorResponse{
			Message: "Failed Request", // Generic message
			Code:    response.Code,
		}, nil
	default:
		return nil, fmt.Errorf("unexpected response: %s", r.Kind())
	}
}
