package tcp

import (
	"LetDD/uint128"

	"testing"
)

func TestRequests(t *testing.T) {
	tests := []struct {
		name     string
		request  Request
		expected string
	}{
		{
			name:     "Magic Request",
			request:  MagicRequest{},
			expected: ";\n",
		},
		{
			name: "Add Account Request",
			request: AddAccountRequest{
				WalID:   0,
				Credits: uint128.Uint128{High: 0, Low: 5},
				Debits:  uint128.Uint128{High: 0, Low: 10},
				Flags:   7,
			},
			expected: "+0 5 10 7\n",
		},
		{
			name: "Make Transfer Request",
			request: MakeTransferRequest{
				WalID:  2,
				FromID: 4001,
				ToID:   1000,
				Amount: uint128.Uint128{High: 0, Low: 1020},
			},
			expected: "%2 4001 1000 1020\n",
		},
		{
			name: "Get Account Request",
			request: GetAccountRequest{
				AccountID: 834858,
			},
			expected: "?834858\n",
		},
		{
			name:     "Count Entries Request",
			request:  CountEntriesRequest{},
			expected: "#\n",
		},
		{
			name: "Update Account Request",
			request: UpdateAccountRequest{
				WalID:     5,
				AccountID: 1245,
				Flags:     3,
			},
			expected: "=5 1245 3\n",
		},
		{
			name:     "Close Request",
			request:  CloseRequest{},
			expected: ".\n",
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			result := string(tt.request.Encode())
			if result != tt.expected {
				t.Fatalf("expected %q, got %q", tt.expected, result)
			}
		})
	}
}
