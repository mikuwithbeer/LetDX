package http

import (
	"LetDD/tcp"

	"github.com/labstack/echo/v5"
)

// Handles GET requests to retrieve specific account information.
func (s *Server) GetAccount(ctx *echo.Context) error {
	accountId, err := echo.PathParam[uint64](ctx, "id")
	if err != nil {
		return err
	}

	return s.Communicate(ctx, tcp.GetAccountRequest{
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

	return s.Communicate(ctx, tcp.UpdateAccountRequest{
		WalID:     s.Client.WalID(),
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

	return s.Communicate(ctx, tcp.AddAccountRequest{
		WalID:   s.Client.WalID(),
		Credits: *postAccount.Credits,
		Debits:  *postAccount.Debits,
		Flags:   *postAccount.Flags,
	})
}

// Handles POST requests to create a new transfer.
func (s *Server) PostTransfer(ctx *echo.Context) error {
	postTransfer, err := BindAndValidate[PostTransferRequest](ctx)
	if err != nil {
		return err
	}

	return s.Communicate(ctx, tcp.MakeTransferRequest{
		WalID:  s.Client.WalID(),
		FromID: *postTransfer.FromID,
		ToID:   *postTransfer.ToID,
		Amount: *postTransfer.Amount,
	})
}
