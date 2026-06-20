package http

import (
	"LetDD/tcp"

	"github.com/labstack/echo/v5"
)

func (s *Server) getAccount(ctx *echo.Context) error {
	accountId, err := echo.PathParam[uint64](ctx, "id")
	if err != nil {
		return err
	}

	clientResponse, err := s.Client.Communicate(tcp.GetAccountRequest{AccountID: accountId})
	if err != nil {
		return err
	}

	serverResponse, err := ToResponse(clientResponse)
	if err != nil {
		return err
	}

	return serverResponse.ToJSON(ctx)
}

func (s *Server) updateAccount(ctx *echo.Context) error {
	accountId, err := echo.PathParam[uint64](ctx, "id")
	if err != nil {
		return err
	}

	var updateAccount UpdateAccountRequest

	err = ctx.Bind(&updateAccount)
	if err != nil {
		return err
	}

	err = ctx.Validate(&updateAccount)
	if err != nil {
		return err
	}

	clientResponse, err := s.Client.Communicate(tcp.UpdateAccountRequest{
		WalID:     s.Client.WalID(),
		AccountID: accountId,
		Flags:     *updateAccount.Flags,
	})
	if err != nil {
		return err
	}

	serverResponse, err := ToResponse(clientResponse)
	if err != nil {
		return err
	}

	return serverResponse.ToJSON(ctx)
}

func (s *Server) postAccount(ctx *echo.Context) error {
	var postAccount PostAccountRequest

	err := ctx.Bind(&postAccount)
	if err != nil {
		return err
	}

	err = ctx.Validate(&postAccount)
	if err != nil {
		return err
	}

	clientResponse, err := s.Client.Communicate(tcp.AddAccountRequest{
		WalID:   s.Client.WalID(),
		Credits: *postAccount.Credits,
		Debits:  *postAccount.Debits,
		Flags:   *postAccount.Flags,
	})
	if err != nil {
		return err
	}

	serverResponse, err := ToResponse(clientResponse)
	if err != nil {
		return err
	}

	return serverResponse.ToJSON(ctx)
}

func (s *Server) postTransfer(ctx *echo.Context) error {
	var postTransfer PostTransferRequest

	err := ctx.Bind(&postTransfer)
	if err != nil {
		return err
	}

	err = ctx.Validate(&postTransfer)
	if err != nil {
		return err
	}

	clientResponse, err := s.Client.Communicate(tcp.MakeTransferRequest{
		WalID:  s.Client.WalID(),
		FromID: *postTransfer.FromID,
		ToID:   *postTransfer.ToID,
		Amount: *postTransfer.Amount,
	})
	if err != nil {
		return err
	}

	serverResponse, err := ToResponse(clientResponse)
	if err != nil {
		return err
	}

	return serverResponse.ToJSON(ctx)
}
