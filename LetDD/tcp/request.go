package tcp

import (
	"LetDD/uint128"

	"fmt"
)

// Defines the interface for TCP requests.
type Request interface {
	// Encodes the request into a byte slice suitable for transmission over TCP.
	Encode() []byte
}

// Represents a request to initiate a handshake with the server.
type MagicRequest struct{}

func (MagicRequest) Encode() []byte { return []byte(";\n") }

// Represents a request to add a new account to the server.
type AddAccountRequest struct {
	WalID   uint64
	Credits uint128.Uint128
	Debits  uint128.Uint128
	Flags   uint8
}

func (r AddAccountRequest) Encode() []byte {
	return fmt.Appendf(nil, "+%d %s %s %d\n", r.WalID, r.Credits, r.Debits, r.Flags)
}

// Represents a request to make a transfer between accounts.
type MakeTransferRequest struct {
	WalID  uint64
	FromID uint64
	ToID   uint64
	Amount uint128.Uint128
}

func (r MakeTransferRequest) Encode() []byte {
	return fmt.Appendf(nil, "%%%d %d %d %s\n", r.WalID, r.FromID, r.ToID, r.Amount)
}

// Represents a request to retrieve account details from the server.
type GetAccountRequest struct {
	AccountID uint64
}

func (r GetAccountRequest) Encode() []byte { return fmt.Appendf(nil, "?%d\n", r.AccountID) }

// Represents a request to count the number of entries in the server.
type CountEntriesRequest struct{}

func (r CountEntriesRequest) Encode() []byte { return []byte("#\n") }

// Represents a request to update an existing account on the server.
type UpdateAccountRequest struct {
	WalID     uint64
	AccountID uint64
	Flags     uint8
}

func (r UpdateAccountRequest) Encode() []byte {
	return fmt.Appendf(nil, "=%d %d %d\n", r.WalID, r.AccountID, r.Flags)
}

// Represents a request to close the connection with the server.
type CloseRequest struct{}

func (r CloseRequest) Encode() []byte { return []byte(".\n") }
