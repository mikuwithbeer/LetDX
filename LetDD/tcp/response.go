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
	GetAccountResponseKind   = "ACC"
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

type GetAccountResponse struct {
	Credits uint128.Uint128
	Debits  uint128.Uint128
	Flags   uint8
}

func (GetAccountResponse) Kind() string { return GetAccountResponseKind }

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

	parameters := bytes.SplitN(data, []byte(" "), 4)

	switch {
	case bytes.HasPrefix(data, []byte(AddAccountResponseKind)):
		if len(parameters) != 2 {
			return nil, fmt.Errorf("invalid add account response format")
		}

		accountID, err := strconv.ParseUint(string(parameters[1]), 16, 64)
		if err != nil {
			return nil, err
		}

		return AddAccountResponse{AccountID: accountID}, nil
	case bytes.HasPrefix(data, []byte(GetAccountResponseKind)):
		if len(parameters) != 4 {
			return nil, fmt.Errorf("invalid get account response format")
		}

		credits, err := uint128.Parse(string(parameters[1]), 16)
		if err != nil {
			return nil, err
		}

		debits, err := uint128.Parse(string(parameters[2]), 16)
		if err != nil {
			return nil, err
		}

		flags, err := strconv.ParseUint(string(parameters[3]), 16, 8)
		if err != nil {
			return nil, err
		}

		return GetAccountResponse{Credits: credits, Debits: debits, Flags: uint8(flags)}, nil
	case bytes.HasPrefix(data, []byte(CountEntriesResponseKind)):
		if len(parameters) != 2 {
			return nil, fmt.Errorf("invalid count entries response format")
		}

		count, err := strconv.ParseUint(string(parameters[1]), 16, 64)
		if err != nil {
			return nil, err
		}

		return CountEntriesResponse{Count: count}, nil
	case bytes.HasPrefix(data, []byte(ErrorResponseKind)):
		if len(parameters) != 2 {
			return nil, fmt.Errorf("invalid error response format")
		}

		code, err := strconv.ParseUint(string(parameters[1]), 16, 16)
		if err != nil {
			return nil, err
		}

		return ErrorResponse{Code: uint16(code)}, nil
	}

	return nil, fmt.Errorf("unknown response type")
}
