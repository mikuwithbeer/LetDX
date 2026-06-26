package http

import (
	"LetDD/tcp"

	"net/http"

	"github.com/labstack/echo/v5"
)

// Handles GET requests to retrieve specific account information.
func (s *Server) GetAccount(ctx *echo.Context) error {
	accountId, err := echo.PathParam[uint64](ctx, "id")
	if err != nil {
		return err
	}

	return s.Communicate(ctx, &tcp.GetAccountRequest{
		AccountID: accountId,
	})
}

// Handles PUT requests to update specific account's flags.
func (s *Server) UpdateAccount(ctx *echo.Context) error {
	accountId, err := echo.PathParam[uint64](ctx, "id")
	if err != nil {
		return err
	}

	updateAccount, err := BindAndValidate[UpdateAccountRequest](ctx)
	if err != nil {
		return err
	}

	return s.Communicate(ctx, &tcp.UpdateAccountRequest{
		AccountID: accountId,
		Flags:     *updateAccount.Flags,
	})
}

// Handles POST requests to create a new account.
func (s *Server) PostAccount(ctx *echo.Context) error {
	postAccount, err := BindAndValidate[PostAccountRequest](ctx)
	if err != nil {
		return err
	}

	return s.Communicate(ctx, &tcp.AddAccountRequest{
		Credits: *postAccount.Credits,
		Debits:  *postAccount.Debits,
		Flags:   *postAccount.Flags,
	})
}

// Handles POST requests to create a new transfer.
func (s *Server) PostTransfer(ctx *echo.Context) error {
	var makeTransfer *tcp.MakeTransferRequest

	switch transferResult := ctx.Get("transfer").(type) {
	case *PostTransferRequest:
		// The request has been already validated from the rate limiter middleware.
		makeTransfer = &tcp.MakeTransferRequest{
			FromID: *transferResult.FromID,
			ToID:   *transferResult.ToID,
			Amount: *transferResult.Amount,
		}
	case nil:
		// No ratelimiter was set, so we bind and validate the request directly.
		postTransfer, err := BindAndValidate[PostTransferRequest](ctx)
		if err != nil {
			return err
		}

		makeTransfer = &tcp.MakeTransferRequest{
			FromID: *postTransfer.FromID,
			ToID:   *postTransfer.ToID,
			Amount: *postTransfer.Amount,
		}
	default:
		return ctx.JSON(http.StatusInternalServerError, map[string]string{"error": "Unreachable Error"})
	}

	return s.Communicate(ctx, makeTransfer)
}
