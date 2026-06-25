package main

import (
	"LetDD/config"
	"LetDD/http"
	"LetDD/tcp"

	"context"
	"errors"
	"log/slog"
	"os"
	"os/signal"
	"syscall"
	"time"
)

func init() {
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

	handler := slog.NewJSONHandler(os.Stdout, &slog.HandlerOptions{
		Level:       slog.LevelInfo,
		ReplaceAttr: replaceAttr,
	})

	slog.SetDefault(slog.New(handler))
}

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
	return server.Start(ctx)
}

func main() {
	ctx := context.Background()
	if err := run(ctx); err != nil {
		slog.Error("Failed to run application", slog.Any("error", err))
		os.Exit(1)
	}
}
