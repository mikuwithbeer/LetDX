package config

import (
	"LetDD/supervisor"

	"fmt"
	"strconv"
	"strings"
	"time"

	"github.com/labstack/echo/v5/middleware"
)

// Parses a rate limiter configuration string.
// The expected format of the input string is `<requests>/<window>[/<burst>][/<expires>]`.
func ParseRateLimiter(input string) (middleware.RateLimiterMemoryStoreConfig, error) {
	ratelimiter := middleware.RateLimiterMemoryStoreConfig{}

	if input == "" {
		return ratelimiter, nil
	}

	parts := strings.Split(input, "/")
	if len(parts) < 2 || len(parts) > 4 {
		return ratelimiter, fmt.Errorf("invalid format. expected: <requests>/<window>[/<burst>][/<expires>]")
	}

	requests, err := strconv.Atoi(parts[0])
	if err != nil || requests <= 0 {
		return ratelimiter, fmt.Errorf("invalid requests value: %q", parts[0])
	}

	window, err := time.ParseDuration(parts[1])
	if err != nil || window <= 0 {
		return ratelimiter, fmt.Errorf("invalid time window: %q", parts[1])
	}

	// Parse burst if provided, otherwise default to requests.
	burst := requests
	if len(parts) == 3 {
		burst, err = strconv.Atoi(parts[2])
		if err != nil || burst <= 0 {
			return ratelimiter, fmt.Errorf("invalid burst value: %q", parts[2])
		}
	}

	// Parse expires if provided, otherwise default to 3 times the window.
	expires := window * 3
	if len(parts) == 4 && parts[3] != "" {
		expires, err = time.ParseDuration(parts[3])
		if err != nil || expires <= 0 {
			return ratelimiter, fmt.Errorf("invalid expires value: %q", parts[3])
		}
	}

	// Calculate and set the configuration.
	ratelimiter.Rate = float64(requests) / window.Seconds()
	ratelimiter.Burst = burst
	ratelimiter.ExpiresIn = expires

	return ratelimiter, nil
}

// Parses a supervisor configuration string.
// The expected format of the input string is `<attempt>/<maximum>/<minimum>`.
func ParseSupervisor(input string) (supervisor.Supervisor, error) {
	supervisor := supervisor.Supervisor{}

	if input == "" {
		return supervisor, nil
	}

	parts := strings.Split(input, "/")
	if len(parts) < 2 || len(parts) > 3 {
		return supervisor, fmt.Errorf("invalid format. expected: <attempt>/<maximum>/<minimum>")
	}

	attempt, err := strconv.ParseUint(parts[0], 10, 64)
	if err != nil || attempt <= 0 {
		return supervisor, fmt.Errorf("invalid attempt value: %q", parts[0])
	}

	maximum, err := time.ParseDuration(parts[1])
	if err != nil || maximum <= 0 {
		return supervisor, fmt.Errorf("invalid maximum value: %q", parts[1])
	}

	minimum, err := time.ParseDuration(parts[2])
	if err != nil || minimum <= 0 {
		return supervisor, fmt.Errorf("invalid minimum value: %q", parts[2])
	}

	supervisor.CancelAfter = attempt
	supervisor.MaximumDelay = maximum
	supervisor.MinimumDelay = minimum

	return supervisor, nil
}
