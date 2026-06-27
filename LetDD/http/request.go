package http

import (
	"LetDD/uint128"

	"github.com/go-playground/validator/v10"
	"github.com/labstack/echo/v5"
)

// Represents the structure of a request to create a new account.
type PostAccountRequest struct {
	Credits *uint128.Uint128 `json:"credits" validate:"required"`
	Debits  *uint128.Uint128 `json:"debits" validate:"required"`
	Flags   Flags            `json:"flags" validate:"required"`
}

// Represents the structure of a request to update an existing account's flags.
type UpdateAccountRequest struct {
	Flags Flags `json:"flags" validate:"required"`
}

// Represents the structure of a request to create a new transfer.
type PostTransferRequest struct {
	FromID *uint64          `json:"from" validate:"required"`
	ToID   *uint64          `json:"to" validate:"required"`
	Amount *uint128.Uint128 `json:"amount" validate:"required"`
}

// Acts as an adapter between the Echo's validation mechanism and validator library.
type RequestValidator struct {
	validator *validator.Validate
}

// Creates a new instance of `RequestValidator` with a fresh validator.
func NewRequestValidator() *RequestValidator {
	return &RequestValidator{
		validator: validator.New(),
	}
}

// Validates the provided request structure against the defined validation rules.
func (v *RequestValidator) Validate(i any) error {
	if err := v.validator.Struct(i); err != nil {
		return echo.ErrBadRequest.Wrap(err)
	}

	return nil
}

// Binds the incoming request data to the provided structure and validates it.
func BindAndValidate[T any](ctx *echo.Context) (*T, error) {
	var request T

	if err := ctx.Bind(&request); err != nil {
		return nil, err
	}

	if err := ctx.Validate(&request); err != nil {
		return nil, err
	}

	return &request, nil
}
