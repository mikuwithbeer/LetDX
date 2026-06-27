package supervisor

import (
	"context"
	"log/slog"
	"time"
)

// Manages the retry logic and exponential backoff timing for a specific operation.
type Supervisor struct {
	CancelAfter uint64 // Maximum number of retry attempts before giving up
	CurrentTry  uint64 // Current retry attempt count

	MaximumDelay time.Duration // Maximum delay between retries
	MinimumDelay time.Duration // Minimum delay between retries
}

// Creates a new `Supervisor` instance with the specified parameters.
func NewSupervisor(cancelAfter uint64, maximumDelay, minimumDelay time.Duration) *Supervisor {
	return &Supervisor{
		CancelAfter: cancelAfter,
		CurrentTry:  0,

		MaximumDelay: maximumDelay,
		MinimumDelay: minimumDelay,
	}
}

// Attempts to perform an operation.
// Returns true if the operation can proceed, false otherwise.
func (s *Supervisor) Try(ctx context.Context) bool {
	// Abort immediately if the context is already done.
	if ctx.Err() != nil {
		return false
	}

	// The first attempt requires no backoff delay.
	if s.CurrentTry == 0 {
		s.CurrentTry++
		return true
	}

	// Stop retrying if we have reached the allowed maximum attempts.
	if s.CurrentTry >= s.CancelAfter {
		return false
	}

	backoff := s.CalculateBackoff()

	slog.Log(ctx, slog.LevelInfo, "SUPERVISOR",
		slog.Duration("backoff", backoff),
		slog.Uint64("try", s.CurrentTry),
		slog.Uint64("max_tries", s.CancelAfter),
	)

	timer := time.NewTimer(backoff)
	defer timer.Stop()

	// Wait for either the backoff timer to expire or the context to be canceled.
	select {
	case <-ctx.Done():
		return false
	case <-timer.C:
		s.CurrentTry++
		return true
	}
}

// Calculates the backoff duration.
func (s *Supervisor) CalculateBackoff() time.Duration {
	backoff := (time.Duration(1) << (s.CurrentTry - 1)) * s.MinimumDelay
	return min(max(backoff, s.MinimumDelay), s.MaximumDelay)
}

// Resets the retry count.
func (s *Supervisor) Reset() {
	s.CurrentTry = 0
}
