package main

import (
	"LetDD/tcp"
	"fmt"
)

func main() {
	addAccount := tcp.AddAccount{WalID: 0, Balance: 1000, Flags: 0}
	fmt.Printf("Encoded: %s", addAccount.Encode())
}
