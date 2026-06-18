package tcp

import (
	"LetDD/uint128"

	"fmt"
)

type Request interface {
	Encode() []byte
}

type MagicRequest struct{}

func (MagicRequest) Encode() []byte { return []byte(";\n") }

type AddAccountRequest struct {
	WalID   uint64
	Credits uint128.Uint128
	Debits  uint128.Uint128
	Flags   uint8
}

func (r AddAccountRequest) Encode() []byte {
	return fmt.Appendf(nil, "+%d %s %s %d\n", r.WalID, r.Credits, r.Debits, r.Flags)
}

type MakeTransferRequest struct {
	WalID  uint64
	FromID uint64
	ToID   uint64
	Amount uint128.Uint128
}

func (r MakeTransferRequest) Encode() []byte {
	return fmt.Appendf(nil, "%%%d %d %d %s\n", r.WalID, r.FromID, r.ToID, r.Amount)
}

type GetAccountRequest struct {
	AccountID uint64
}

func (r GetAccountRequest) Encode() []byte { return fmt.Appendf(nil, "?%d\n", r.AccountID) }

type CountEntriesRequest struct{}

func (r CountEntriesRequest) Encode() []byte { return []byte("#\n") }

type UpdateAccountRequest struct {
	WalID     uint64
	AccountID uint64
	Flags     uint8
}

func (r UpdateAccountRequest) Encode() []byte {
	return fmt.Appendf(nil, "=%d %d %d\n", r.WalID, r.AccountID, r.Flags)
}

type CloseRequest struct{}

func (r CloseRequest) Encode() []byte { return []byte(".\n") }
