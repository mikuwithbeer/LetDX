package main

import (
	"LetDD/tcp"

	"fmt"
	"time"
)

func main() {
	client, err := tcp.NewClient(55543)
	if err != nil {
		fmt.Println("connect error:", err)
		return
	}

	defer client.Close()

	client.Start()
	fmt.Println("client started")

	go func() {
		for {
			response := client.Receive()
			fmt.Printf("response: %#v\n", response)
		}
	}()

	go func() {
		for err := range client.Errors {
			fmt.Printf("error: %s", err)
		}
	}()

	client.Send(tcp.MagicRequest{})
	time.Sleep(500 * time.Millisecond)

	client.Send(tcp.CountEntriesRequest{})
	time.Sleep(500 * time.Millisecond)

	client.Send(tcp.AddAccountRequest{
		WalID:   0,
		Balance: 1000,
		Flags:   1,
	})
	time.Sleep(500 * time.Millisecond)

	client.Send(tcp.AddAccountRequest{
		WalID:   1,
		Balance: 1000,
		Flags:   2,
	})
	time.Sleep(500 * time.Millisecond)

	client.Send(tcp.MakeTransferRequest{
		WalID:  2,
		FromID: 0,
		ToID:   1,
		Amount: 500,
	})
	time.Sleep(500 * time.Millisecond)
	client.Send(tcp.GetBalanceRequest{
		AccountID: 0,
	})
	time.Sleep(500 * time.Millisecond)

	client.Send(tcp.GetBalanceRequest{
		AccountID: 1,
	})
	time.Sleep(500 * time.Millisecond)

	client.Send(tcp.CloseRequest{})
	time.Sleep(500 * time.Millisecond)
}
