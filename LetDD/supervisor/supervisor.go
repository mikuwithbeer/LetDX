package supervisor

import (
	"context"
	"log/slog"
	"time"
)

// Manages the retry logic and exponential backoff timing for a specific operation.
type Supervisor struct {
	cancelAfter uint64 // Maximum number of retry attempts before giving up
	currentTry  uint64 // Current retry attempt count

	minimumDelay time.Duration // Minimum delay between retries
	maximumDelay time.Duration // Maximum delay between retries
}

// Creates a new `Supervisor` instance with the specified parameters.
func NewSupervisor(cancelAfter uint64, minimumDelay, maximumDelay time.Duration) *Supervisor {
	return &Supervisor{
		cancelAfter: cancelAfter,
		currentTry:  0,

		minimumDelay: minimumDelay,
		maximumDelay: maximumDelay,
	}
}

// Tries to execute the operation, returning true if successful or false if the maximum number of attempts has been reached.
func (s *Supervisor) Try(ctx context.Context) bool {
	// Abort immediately if the context is already done.
	if ctx.Err() != nil {
		return false
	}

	// The first attempt requires no backoff delay.
	if s.currentTry == 0 {
		s.currentTry++
		return true
	}

	// Stop retrying if we have reached the allowed maximum attempts.
	if s.currentTry >= s.cancelAfter {
		return false
	}

	backoff := s.CalculateBackoff()

	slog.Log(ctx, slog.LevelInfo, "SUPERVISOR",
		slog.Duration("backoff", backoff),
		slog.Uint64("try", s.currentTry),
		slog.Uint64("max_tries", s.cancelAfter),
	)

	timer := time.NewTimer(backoff)
	defer timer.Stop()

	// Wait for either the backoff timer to expire or the context to be canceled.
	select {
	case <-ctx.Done():
		return false
	case <-timer.C:
		s.currentTry++
		return true
	}
}

// Calculates the backoff duration.
func (s *Supervisor) CalculateBackoff() time.Duration {
	backoff := (time.Duration(1) << (s.currentTry - 1)) * s.minimumDelay
	return min(max(backoff, s.minimumDelay), s.maximumDelay)
}

// Resets the retry count.
func (s *Supervisor) Reset() {
	s.currentTry = 0
}
