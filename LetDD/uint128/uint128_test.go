package uint128

import (
	"fmt"
	"testing"
)

func TestUint128(t *testing.T) {
	tests := []struct {
		name          string
		high          uint64
		low           uint64
		base10        string
		base16        string
		failOnPurpose bool
	}{
		{
			name:   "Zero",
			high:   0,
			low:    0,
			base10: "0",
			base16: "0",
		},
		{
			name:   "Small Number",
			high:   0,
			low:    3242930,
			base10: "3242930",
			base16: "317BB2",
		},
		{
			name:   "Max u64",
			high:   0,
			low:    18446744073709551615,
			base10: "18446744073709551615",
			base16: "FFFFFFFFFFFFFFFF",
		},
		{
			name:   "Exceeds u64",
			high:   1,
			low:    1,
			base10: "18446744073709551617",
			base16: "10000000000000001",
		},
		{
			name:   "Large Number",
			high:   5,
			low:    42,
			base10: "92233720368547758122",
			base16: "5000000000000002A",
		},
		{
			name:   "Max u128",
			high:   0xFFFFFFFFFFFFFFFF,
			low:    0xFFFFFFFFFFFFFFFF,
			base10: "340282366920938463463374607431768211455",
			base16: "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
		},
		{
			name:          "Empty String",
			base10:        "",
			failOnPurpose: true,
		},
		{
			name:          "Invalid Format",
			base10:        "not-a-number",
			failOnPurpose: true,
		},
		{
			name:          "Exceeds u128",
			base10:        "340282366920938463463374607431768211456",
			failOnPurpose: true,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if tt.failOnPurpose {
				_, err := Parse(tt.base10, 10)
				if err == nil {
					t.Fatalf("expected error for input %q, got nil", tt.base10)
				}
				return
			}

			u := Uint128{High: tt.high, Low: tt.low}

			if got := u.String(); got != tt.base10 {
				t.Errorf("String() expected %q, got %q", tt.base10, got)
			}

			parsed10, err := Parse(tt.base10, 10)
			if err != nil {
				t.Errorf("Parse() Base 10 unexpected error: %v", err)
			} else if parsed10 != u {
				t.Errorf("Parse() Base 10 expected %v, got %v", u, parsed10)
			}

			parsed16, err := Parse(tt.base16, 16)
			if err != nil {
				t.Errorf("Parse() Base 16 unexpected error: %v", err)
			} else if parsed16 != u {
				t.Errorf("Parse() Base 16 expected %v, got %v", u, parsed16)
			}

			expectedJSON := fmt.Sprintf("%q", tt.base10)
			jsonBytes, err := u.MarshalJSON()
			if err != nil {
				t.Errorf("MarshalJSON() unexpected error: %v", err)
			} else if string(jsonBytes) != expectedJSON {
				t.Errorf("MarshalJSON() expected %s, got %s", expectedJSON, string(jsonBytes))
			}

			var unmarshaled Uint128
			err = unmarshaled.UnmarshalJSON([]byte(expectedJSON))
			if err != nil {
				t.Errorf("UnmarshalJSON() unexpected error: %v", err)
			} else if unmarshaled != u {
				t.Errorf("UnmarshalJSON() expected %v, got %v", u, unmarshaled)
			}
		})
	}
}
