package main

import (
	"LetDD/config"
	"LetDD/http"
	"LetDD/tcp"

	"log"
)

func main() {
	config := &config.Config{}
	config.Collect()

	client, err := tcp.NewClient(*config.ConnectAddress)
	if err != nil {
		log.Fatalf("Failed to create TCP client: %v", err)
	}

	server := http.NewServer(client, config)
	err = server.Start()
	if err != nil {
		log.Fatalf("Failed to create HTTP server: %v", err)
	}

	client.Close()
}
