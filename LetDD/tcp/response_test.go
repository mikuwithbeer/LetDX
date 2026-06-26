package tcp

import (
	"LetDD/uint128"

	"reflect"
	"testing"
)

func TestResponse(t *testing.T) {
	tests := []struct {
		name          string
		input         []byte
		expected      Response
		failOnPurpose bool
	}{
		{
			name:          "Magic Response",
			input:         []byte("LET"),
			expected:      MagicResponse{},
			failOnPurpose: false,
		},
		{
			name:          "Ok Response",
			input:         []byte("OKE\n"),
			expected:      OkResponse{},
			failOnPurpose: false,
		},
		{
			name:          "Add Account Response",
			input:         []byte("AID 0A3850F"),
			expected:      AddAccountResponse{AccountID: 0x0A3850F},
			failOnPurpose: false,
		},
		{
			name:  "Get Account Response",
			input: []byte("ACC 01 DEADBEEF 0A"),
			expected: GetAccountResponse{
				Credits: uint128.Uint128{High: 0, Low: 0x01},
				Debits:  uint128.Uint128{High: 0, Low: 0xDEADBEEF},
				Flags:   0x0A,
			},
			failOnPurpose: false,
		},
		{
			name:          "Count Database Response",
			input:         []byte("LEN 4F2 1A3"),
			expected:      CountDatabaseResponse{Accounts: 0x4F2, Transactions: 0x1A3},
			failOnPurpose: false,
		},
		{
			name:          "Error Response",
			input:         []byte("ERR 03D1"),
			expected:      ErrorResponse{Code: 0x03D1},
			failOnPurpose: false,
		},
		{
			name:          "Empty Response",
			input:         []byte("   \n"),
			expected:      nil,
			failOnPurpose: true,
		},
		{
			name:          "Unknown Type",
			input:         []byte("FOO 123"),
			expected:      nil,
			failOnPurpose: true,
		},
		{
			name:          "Missing Parameter",
			input:         []byte("AID"),
			expected:      nil,
			failOnPurpose: true,
		},
		{
			name:          "Invalid Parameter Format",
			input:         []byte("AID idk"),
			expected:      nil,
			failOnPurpose: true,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			result, err := ParseResponse(tt.input)

			if (err != nil) != tt.failOnPurpose {
				t.Fatalf("expected error: %v, got error: %v", tt.failOnPurpose, err)
			}

			if !reflect.DeepEqual(result, tt.expected) {
				t.Fatalf("expected %#v, got %#v", tt.expected, result)
			}
		})
	}
}
