package tcp

import "fmt"

type Command interface {
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

func (c AddAccount) Encode() []byte {
	return fmt.Appendf(nil, "+%d %d %d\n", c.WalID, c.Balance, c.Flags)
}

func (c MakeTransfer) Encode() []byte {
	return fmt.Appendf(nil, "%%%d %d %d %d\n", c.WalID, c.FromID, c.ToID, c.Amount)
}

func (c GetBalance) Encode() []byte {
	return fmt.Appendf(nil, "?%d\n", c.AccountID)
}

func (CountEntries) Encode() []byte {
	return []byte("#\n")
}

func (c UpdateAccount) Encode() []byte {
	return fmt.Appendf(nil, "=%d %d %d\n", c.WalID, c.AccountID, c.Flags)
}

func (Close) Encode() []byte {
	return []byte(".\n")
}
