package http

import (
	"LetDD/uint128"

	"github.com/go-playground/validator/v10"
	"github.com/labstack/echo/v5"
)

type PostAccountRequest struct {
	Credits *uint128.Uint128 `json:"credits" validate:"required"`
	Debits  *uint128.Uint128 `json:"debits" validate:"required"`
	Flags   *uint8           `json:"flags" validate:"required"`
}

type UpdateAccountRequest struct {
	Flags *uint8 `json:"flags" validate:"required"`
}

type PostTransferRequest struct {
	FromID *uint64          `json:"from" validate:"required"`
	ToID   *uint64          `json:"to" validate:"required"`
	Amount *uint128.Uint128 `json:"amount" validate:"required"`
}

type RequestValidator struct {
	validator *validator.Validate
}

func NewRequestValidator() *RequestValidator {
	return &RequestValidator{validator: validator.New()}
}

func (v *RequestValidator) Validate(i any) error {
	if err := v.validator.Struct(i); err != nil {
		return echo.ErrBadRequest.Wrap(err)
	}

	return nil
}
