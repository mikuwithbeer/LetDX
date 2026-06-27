package supervisor

import (
	"context"
	"testing"
	"time"
)

func TestNewSupervisor(t *testing.T) {
	s := NewSupervisor(5, 100*time.Millisecond, 10*time.Millisecond)

	if s.CancelAfter != 5 {
		t.Errorf("Expected CancelAfter to be 5, got %d", s.CancelAfter)
	}

	if s.MinimumDelay != 10*time.Millisecond {
		t.Errorf("Expected MinimumDelay to be 10ms, got %v", s.MinimumDelay)
	}

	if s.MaximumDelay != 100*time.Millisecond {
		t.Errorf("Expected MaximumDelay to be 100ms, got %v", s.MaximumDelay)
	}

	if s.CurrentTry != 0 {
		t.Errorf("Expected CurrentTry to be 0, got %d", s.CurrentTry)
	}
}

func TestCalculateBackoff(t *testing.T) {
	minDelay := 10 * time.Millisecond
	maxDelay := 50 * time.Millisecond
	s := NewSupervisor(10, maxDelay, minDelay)

	tests := []struct {
		try      uint64
		expected time.Duration
	}{
		{try: 1, expected: 10 * time.Millisecond},
		{try: 2, expected: 20 * time.Millisecond},
		{try: 3, expected: 40 * time.Millisecond},
		{try: 4, expected: 50 * time.Millisecond},
		{try: 5, expected: 50 * time.Millisecond},
	}

	for _, tt := range tests {
		s.CurrentTry = tt.try
		got := s.CalculateBackoff()
		if got != tt.expected {
			t.Errorf("For try %d, expected backoff %v, got %v", tt.try, tt.expected, got)
		}
	}
}

func TestMaxAttempts(t *testing.T) {
	ctx := context.Background()
	s := NewSupervisor(3, 1*time.Millisecond, 5*time.Millisecond)

	if !s.Try(ctx) {
		t.Error("Attempt 1 failed, expected true")
	}

	if !s.Try(ctx) {
		t.Error("Attempt 2 failed, expected true")
	}

	if !s.Try(ctx) {
		t.Error("Attempt 3 failed, expected true")
	}

	if s.Try(ctx) {
		t.Error("Attempt 4 succeeded, expected false due to max tries")
	}
}
