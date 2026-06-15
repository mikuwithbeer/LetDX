package main

import (
	"LetDD/http"
	"LetDD/tcp"
	"LetDD/uint128"

	"fmt"

	"github.com/labstack/echo/v5"
)

func main() {
	client, err := tcp.NewClient("localhost:55543")
	if err != nil {
		panic(err)
	}

	// simple demo, not a real implementation of the server
	server := http.NewServer()
	server.GetAccount = func(ctx *echo.Context) error {
		id, err := echo.QueryParam[uint64](ctx, "id")
		if err != nil {
			return err
		}

		response, err := client.Communicate(tcp.GetBalanceRequest{AccountID: id})
		if err != nil {
			return err
		}

		switch response.Kind() {
		case tcp.GetBalanceResponseKind:
			balance := response.(tcp.GetBalanceResponse).Balance
			return ctx.String(200, fmt.Sprintf("Account ID: %d, Balance: %s", id, balance))
		default:
			return fmt.Errorf("unexpected response: %s", response.Kind())
		}
	}

	server.PostAccount = func(ctx *echo.Context) error {
		balance, err := echo.FormValue[string](ctx, "balance")
		if err != nil {
			return err
		}

		balanceUint128, err := uint128.Parse(balance)
		if err != nil {
			return err
		}

		flags, err := echo.FormValue[uint8](ctx, "flags")
		if err != nil {
			return err
		}

		response, err := client.Communicate(tcp.AddAccountRequest{
			WalID:   client.WalID(),
			Balance: balanceUint128,
			Flags:   flags,
		})
		if err != nil {
			return err
		}

		switch response.Kind() {
		case tcp.AddAccountResponseKind:
			return ctx.String(200, "Account added successfully")
		default:
			return fmt.Errorf("unexpected response: %s", response.Kind())
		}
	}

	server.UpdateAccount = func(ctx *echo.Context) error {
		id, err := echo.FormValue[uint64](ctx, "id")
		if err != nil {
			return err
		}

		flags, err := echo.FormValue[uint8](ctx, "flags")
		if err != nil {
			return err
		}

		response, err := client.Communicate(tcp.UpdateAccountRequest{
			WalID:     client.WalID(),
			AccountID: id,
			Flags:     flags,
		})
		if err != nil {
			return err
		}

		switch response.Kind() {
		case tcp.OkeResponseKind:
			return ctx.String(200, "Account updated successfully")
		default:
			return fmt.Errorf("unexpected response: %s", response.Kind())
		}
	}

	server.PostTransfer = func(ctx *echo.Context) error {
		return ctx.String(200, "PostTransfer")
	}

	if err := server.Start(":8080"); err != nil {
		panic(err)
	}
}
