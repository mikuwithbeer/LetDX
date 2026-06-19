package http

import (
	"LetDD/config"
	"LetDD/tcp"

	"github.com/labstack/echo/v5"
	"github.com/labstack/echo/v5/middleware"
)

type Server struct {
	Echo   *echo.Echo
	Client *tcp.Client
	Config *config.Config
}

func NewServer(client *tcp.Client, config *config.Config) *Server {
	echo := echo.New()
	echo.Validator = NewRequestValidator()

	return &Server{
		Echo:   echo,
		Client: client,
		Config: config,
	}
}

func (s *Server) Start() error {
	if s.Config.ServerToken != nil {
		s.Echo.Use(middleware.KeyAuth(s.Authorization))
	}

	s.Echo.Use(middleware.RequestLogger())

	s.Echo.GET("/accounts/:id", s.getAccount)
	s.Echo.PUT("/accounts/:id", s.updateAccount)
	s.Echo.POST("/accounts", s.postAccount)
	s.Echo.POST("/transfers", s.postTransfer)

	return s.Echo.Start(*s.Config.ServerAddress)
}

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
