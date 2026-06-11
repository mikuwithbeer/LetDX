package main

import (
	"LetDD/tcp"
	"LetDD/uint128"

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

	response, err := client.Communicate(tcp.MagicRequest{})
	if err != nil {
		fmt.Println("error:", err)
		return
	}

	fmt.Printf("response: %#v\n", response)
	time.Sleep(500 * time.Millisecond)

	/*
			response, err = client.Communicate(tcp.AddAccountRequest{
				WalID:   client.WalID(),
				Balance: uint128.Uint128{Low: 1000},
				Flags:   1,
			})
			if err != nil {
				fmt.Println("error:", err)
				return
			}

			fmt.Printf("response: %#v\n", response)
			time.Sleep(500 * time.Millisecond)

			response, err = client.Communicate(tcp.AddAccountRequest{
				WalID:   client.WalID(),
				Balance: uint128.Uint128{Low: 1000},
				Flags:   2,
			})

			if err != nil {
				fmt.Println("error:", err)
				return
			}

		fmt.Printf("response: %#v\n", response)
		time.Sleep(500 * time.Millisecond)
	*/

	response, err = client.Communicate(tcp.MakeTransferRequest{
		WalID:  client.WalID(),
		FromID: 0,
		ToID:   1,
		Amount: uint128.Uint128{Low: 0x32},
	})

	if err != nil {
		fmt.Println("error:", err)
		return
	}

	fmt.Printf("response: %#v\n", response)
	time.Sleep(500 * time.Millisecond)

	response, err = client.Communicate(tcp.GetBalanceRequest{AccountID: 0})
	if err != nil {
		fmt.Println("error:", err)
		return
	}

	fmt.Printf("response: %#v\n", response)
	time.Sleep(500 * time.Millisecond)

	response, err = client.Communicate(tcp.GetBalanceRequest{AccountID: 1})
	if err != nil {
		fmt.Println("error:", err)
		return
	}

	fmt.Printf("response: %#v\n", response)
	time.Sleep(500 * time.Millisecond)

}
