package tcp

import (
	"bytes"
	"testing"
)

func TestCommandEncoding(t *testing.T) {
	tests := []struct {
		name     string
		command  Command
		expected []byte
	}{
		{
			name:     "Magic",
			command:  Magic{},
			expected: []byte(";\n"),
		},
		{
			name: "AddAccount",
			command: AddAccount{
				WalID:   8732478,
				Balance: 255255,
				Flags:   0,
			},
			expected: []byte("+8732478 255255 0\n"),
		},
		{
			name: "MakeTransfer",
			command: MakeTransfer{
				WalID:  999,
				FromID: 20,
				ToID:   3,
				Amount: 637245,
			},
			expected: []byte("%999 20 3 637245\n"),
		},
		{
			name: "GetBalance",
			command: GetBalance{
				AccountID: 7264,
			},
			expected: []byte("?7264\n"),
		},
		{
			name:     "CountEntries",
			command:  CountEntries{},
			expected: []byte("#\n"),
		},
		{
			name: "UpdateAccount",
			command: UpdateAccount{
				WalID:     8384,
				AccountID: 873247682,
				Flags:     1,
			},
			expected: []byte("=8384 873247682 1\n"),
		},
		{
			name:     "Close",
			command:  Close{},
			expected: []byte(".\n"),
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got := tt.command.Encode()
			if !bytes.Equal(got, tt.expected) {
				t.Errorf("Encode() = %q, want %q", got, tt.expected)
			}
		})
	}
}
