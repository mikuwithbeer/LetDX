package tcp

import "fmt"

type Request interface {
	Encode() []byte
}

type WalID uint64
type AccountID uint64
type AccountFlags uint8

type Magic struct{}

type AddAccount struct {
	WalID   WalID
	Balance uint64
	Flags   AccountFlags
}

type MakeTransfer struct {
	WalID  WalID
	FromID AccountID
	ToID   AccountID
	Amount uint64
}

type GetBalance struct {
	AccountID AccountID
}

type CountEntries struct{}

type UpdateAccount struct {
	WalID     WalID
	AccountID AccountID
	Flags     AccountFlags
}

type Close struct{}

func (Magic) Encode() []byte {
	return []byte(";\n")
}

func (r AddAccount) Encode() []byte {
	return fmt.Appendf(nil, "+%d %d %d\n", r.WalID, r.Balance, r.Flags)
}

func (r MakeTransfer) Encode() []byte {
	return fmt.Appendf(nil, "%%%d %d %d %d\n", r.WalID, r.FromID, r.ToID, r.Amount)
}

func (r GetBalance) Encode() []byte {
	return fmt.Appendf(nil, "?%d\n", r.AccountID)
}

func (CountEntries) Encode() []byte {
	return []byte("#\n")
}

func (r UpdateAccount) Encode() []byte {
	return fmt.Appendf(nil, "=%d %d %d\n", r.WalID, r.AccountID, r.Flags)
}

func (Close) Encode() []byte {
	return []byte(".\n")
}
