package supervisor

import (
	"context"
	"log/slog"
	"time"
)

type Supervisor struct {
	cancelAfter uint64
	currentTry  uint64

	minimumDelay time.Duration
	maximumDelay time.Duration
}

func NewSupervisor(cancelAfter uint64, minimumDelay, maximumDelay time.Duration) *Supervisor {
	return &Supervisor{
		cancelAfter: cancelAfter,
		currentTry:  0,

		minimumDelay: minimumDelay,
		maximumDelay: maximumDelay,
	}
}

func (s *Supervisor) Try(ctx context.Context) bool {
	if ctx.Err() != nil {
		return false
	}

	if s.currentTry == 0 {
		s.currentTry++
		return true
	}

	if s.currentTry >= s.cancelAfter {
		return false
	}

	backoff := s.CalculateBackoff()
	slog.Log(ctx, slog.LevelInfo, "Supervisor: retrying",
		slog.Duration("backoff", backoff),
		slog.Uint64("try", s.currentTry),
		slog.Uint64("max_tries", s.cancelAfter),
	)

	timer := time.NewTimer(backoff)
	defer timer.Stop()

	select {
	case <-ctx.Done():
		return false
	case <-timer.C:
		s.currentTry++
		return true
	}
}

func (s *Supervisor) CalculateBackoff() time.Duration {
	backoff := (time.Duration(1) << (s.currentTry - 1)) * s.minimumDelay
	return min(max(backoff, s.minimumDelay), s.maximumDelay)
}

func (s *Supervisor) Reset() {
	s.currentTry = 0
}
