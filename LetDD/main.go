package main

import (
	"LetDD/config"
	"LetDD/http"
	"LetDD/tcp"

	"context"
	"log/slog"
	"os"
	"os/signal"
	"syscall"
	"time"
)

func main() {
	ctx := context.Background()
	if err := run(ctx); err != nil {
		slog.Error("Failed to run application", slog.Any("error", err))
		os.Exit(1)
	}
}

// Initializes the application.
func init() {
	// Configure the logger to output JSON-formatted logs with timestamps.
	replaceAttr := func(_ []string, attr slog.Attr) slog.Attr {
		if attr.Key != slog.TimeKey {
			return attr
		}

		timestamp, ok := attr.Value.Any().(time.Time)
		if !ok {
			return attr
		}

		return slog.String("time", timestamp.Format(time.RFC3339Nano))
	}

	// Setup JSON logger.
	handler := slog.NewJSONHandler(os.Stdout, &slog.HandlerOptions{
		Level:       slog.LevelInfo,
		ReplaceAttr: replaceAttr,
	})

	slog.SetDefault(slog.New(handler))
}

// Runs the main application logic.
func run(ctx context.Context) error {
	// Intercept OS interruption signals (e.g., Ctrl+C, SIGTERM) for graceful shutdown.
	ctx, cancel := signal.NotifyContext(ctx, os.Interrupt, syscall.SIGTERM)
	defer cancel()

	// Load application configuration.
	config := &config.Config{}
	config.Collect()

	// Create a TCP client.
	client := tcp.NewClient(config.ConnectAddress, config.Supervisor)
	defer client.Close()

	// Create and start the HTTP server.
	server := http.NewServer(client, config)
	return server.Start(ctx)
}
