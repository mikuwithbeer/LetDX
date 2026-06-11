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
			reply := client.Receive()
			fmt.Printf("reply: %s", string(reply))
		}
	}()

	go func() {
		for err := range client.Errors {
			fmt.Printf("error: %s", err)
		}
	}()

	client.Send(tcp.Magic{})
	time.Sleep(500 * time.Millisecond)

	client.Send(tcp.CountEntries{})
	time.Sleep(500 * time.Millisecond)

	client.Send(tcp.AddAccount{
		WalID:   0,
		Balance: 1000,
		Flags:   1,
	})
	time.Sleep(500 * time.Millisecond)

	client.Send(tcp.AddAccount{
		WalID:   1,
		Balance: 1000,
		Flags:   2,
	})
	time.Sleep(500 * time.Millisecond)

	client.Send(tcp.MakeTransfer{
		WalID:  2,
		FromID: 0,
		ToID:   1,
		Amount: 500,
	})
	time.Sleep(500 * time.Millisecond)
	client.Send(tcp.GetBalance{
		AccountID: 0,
	})
	time.Sleep(500 * time.Millisecond)

	client.Send(tcp.GetBalance{
		AccountID: 1,
	})
	time.Sleep(500 * time.Millisecond)

	client.Send(tcp.Close{})
	time.Sleep(500 * time.Millisecond)
}
