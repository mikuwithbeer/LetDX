package main

import (
	"LetDD/config"
	"LetDD/http"
	"LetDD/tcp"

	"context"
	"errors"
	"fmt"
	"log"
	stdhttp "net/http"
	"os"
	"os/signal"
	"syscall"
)

func run(ctx context.Context) error {
	ctx, cancel := signal.NotifyContext(ctx, os.Interrupt, syscall.SIGTERM)
	defer cancel()

	config := &config.Config{}
	config.Collect()

	if config.ConnectAddress == nil {
		return errors.New("connection address is missing from configuration")
	}

	client := tcp.NewClient(*config.ConnectAddress)
	defer client.Close()

	server := http.NewServer(client, config)
	err := server.Start(ctx)
	if err != nil && !errors.Is(err, stdhttp.ErrServerClosed) {
		return fmt.Errorf("HTTP server encountered an error: %w", err)
	}

	return nil
}

func main() {
	ctx := context.Background()
	if err := run(ctx); err != nil {
		log.Fatalf("Failed to run application: %v", err)
	}
}
