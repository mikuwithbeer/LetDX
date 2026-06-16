package http

import (
	"LetDD/tcp"
	"LetDD/uint128"
	"fmt"

	"github.com/labstack/echo/v5"
)

type Response interface {
	ToJSON(ctx *echo.Context) error
}

type GetAccountBalanceResponse struct {
	Balance uint128.Uint128 `json:"balance"`
}

func (r GetAccountBalanceResponse) ToJSON(ctx *echo.Context) error {
	return ctx.JSON(200, r)
}

type GetAccountFlagsResponse struct {
	Flags uint8 `json:"flags"`
}

func (r GetAccountFlagsResponse) ToJSON(ctx *echo.Context) error {
	return ctx.JSON(200, r)
}

type PostAccountResponse struct {
	ID uint64 `json:"id"`
}

func (r PostAccountResponse) ToJSON(ctx *echo.Context) error {
	return ctx.JSON(201, r)
}

type OkResponse struct{}

func (r OkResponse) ToJSON(ctx *echo.Context) error {
	return ctx.NoContent(204)
}

type ErrResponse struct {
	Code uint16 `json:"error"`
}

func (r ErrResponse) ToJSON(ctx *echo.Context) error {
	return ctx.JSON(400, r)
}

func ToResponse(r tcp.Response) (Response, error) {
	switch response := r.(type) {
	case tcp.GetBalanceResponse:
		return GetAccountBalanceResponse{Balance: response.Balance}, nil
	case tcp.GetFlagsResponse:
		return GetAccountFlagsResponse{Flags: response.Flags}, nil
	case tcp.AddAccountResponse:
		return PostAccountResponse{ID: response.AccountID}, nil
	case tcp.OkeResponse:
		return OkResponse{}, nil
	case tcp.ErrorResponse:
		return ErrResponse{Code: response.Code}, nil
	default:
		return nil, fmt.Errorf("unexpected response: %s", r.Kind())
	}
}
