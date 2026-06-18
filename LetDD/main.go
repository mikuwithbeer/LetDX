package main

import (
	"LetDD/http"
	"LetDD/tcp"

	"log"
)

func main() {
	client, err := tcp.NewClient("localhost:55543")
	if err != nil {
		log.Fatalf("Failed to create TCP client: %v", err)
	}

	server := http.NewServer(client)
	err = server.Start("localhost:5543")
	if err != nil {
		log.Fatalf("Failed to create HTTP server: %v", err)
	}
}
