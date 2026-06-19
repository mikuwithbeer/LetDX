package main

import (
	"LetDD/http"
	"LetDD/tcp"

	"log"
)

func main() {
	cli := &CLI{}
	cli.Parse()

	client, err := tcp.NewClient(*cli.connect)
	if err != nil {
		log.Fatalf("Failed to create TCP client: %v", err)
	}

	server := http.NewServer(client)
	err = server.Start(*cli.serve)
	if err != nil {
		log.Fatalf("Failed to create HTTP server: %v", err)
	}

	client.Close()
}
