package tcp

import (
	"LetDD/uint128"

	"bytes"
	"fmt"
	"strconv"
)

const (
	MagicResponseKind        = "LET"
	AddAccountResponseKind   = "AID"
	GetBalanceResponseKind   = "BAL"
	CountEntriesResponseKind = "SEC"
	OkeResponseKind          = "OKE"
	ErrorResponseKind        = "ERR"
)

type Response interface {
	Kind() string
}

type MagicResponse struct{}

func (MagicResponse) Kind() string { return MagicResponseKind }

type AddAccountResponse struct {
	AccountID uint64
}

func (AddAccountResponse) Kind() string { return AddAccountResponseKind }

type GetBalanceResponse struct {
	Balance uint128.Uint128
}

func (GetBalanceResponse) Kind() string { return GetBalanceResponseKind }

type CountEntriesResponse struct {
	Count uint64
}

func (CountEntriesResponse) Kind() string { return CountEntriesResponseKind }

type OkeResponse struct{}

func (OkeResponse) Kind() string { return OkeResponseKind }

type ErrorResponse struct {
	Code uint16
}

func (ErrorResponse) Kind() string { return ErrorResponseKind }

func ParseResponse(data []byte) (Response, error) {
	data = bytes.TrimSpace(data)
	if len(data) == 0 {
		return nil, fmt.Errorf("empty response")
	}

	if bytes.Equal(data, []byte(MagicResponseKind)) {
		return MagicResponse{}, nil
	}

	if bytes.Equal(data, []byte(OkeResponseKind)) {
		return OkeResponse{}, nil
	}

	if len(data) < 5 {
		return nil, fmt.Errorf("invalid response format")
	}

	value := data[4:]
	switch {
	case bytes.HasPrefix(data, []byte(AddAccountResponseKind)):
		accountID, err := strconv.ParseUint(string(value), 16, 64)
		if err != nil {
			return nil, err
		}

		return AddAccountResponse{AccountID: accountID}, nil
	case bytes.HasPrefix(data, []byte(GetBalanceResponseKind)):
		balance, err := uint128.Parse(string(value))
		if err != nil {
			return nil, err
		}

		return GetBalanceResponse{Balance: balance}, nil
	case bytes.HasPrefix(data, []byte(CountEntriesResponseKind)):
		count, err := strconv.ParseUint(string(value), 16, 64)
		if err != nil {
			return nil, err
		}

		return CountEntriesResponse{Count: count}, nil
	case bytes.HasPrefix(data, []byte(ErrorResponseKind)):
		code, err := strconv.ParseUint(string(value), 16, 16)
		if err != nil {
			return nil, err
		}

		return ErrorResponse{Code: uint16(code)}, nil
	}

	return nil, fmt.Errorf("unknown response type")
}
