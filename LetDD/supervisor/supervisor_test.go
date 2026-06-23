package supervisor

import (
	"context"
	"testing"
	"time"
)

func TestNewSupervisor(t *testing.T) {
	s := NewSupervisor(5, 10*time.Millisecond, 100*time.Millisecond)

	if s.cancelAfter != 5 {
		t.Errorf("Expected cancelAfter to be 5, got %d", s.cancelAfter)
	}

	if s.minimumDelay != 10*time.Millisecond {
		t.Errorf("Expected minimumDelay to be 10ms, got %v", s.minimumDelay)
	}

	if s.maximumDelay != 100*time.Millisecond {
		t.Errorf("Expected maximumDelay to be 100ms, got %v", s.maximumDelay)
	}

	if s.currentTry != 0 {
		t.Errorf("Expected currentTry to be 0, got %d", s.currentTry)
	}
}

func TestCalculateBackoff(t *testing.T) {
	minDelay := 10 * time.Millisecond
	maxDelay := 50 * time.Millisecond
	s := NewSupervisor(10, minDelay, maxDelay)

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
		s.currentTry = tt.try
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
