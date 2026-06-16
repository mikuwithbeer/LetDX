package main

import (
	"LetDD/http"
	"LetDD/tcp"
)

func main() {
	client, err := tcp.NewClient("localhost:55543")
	if err != nil {
		panic(err)
	}

	server := http.NewServer(client)
	err = server.Start("localhost:5543")
	if err != nil {
		panic(err)
	}
}
