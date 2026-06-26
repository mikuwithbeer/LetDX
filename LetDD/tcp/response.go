package tcp

import (
	"LetDD/uint128"

	"bytes"
	"fmt"
	"strconv"
)

const (
	MagicResponseKind         = "LET" // Response to a handshake request
	AddAccountResponseKind    = "AID" // Response to an add account request
	GetAccountResponseKind    = "ACC" // Response to a get account request
	CountDatabaseResponseKind = "LEN" // Response to a count database request
	OkResponseKind            = "OKE" // Response to a successful operation
	ErrorResponseKind         = "ERR" // Response to an error condition
)

// Defines the interface for TCP responses.
type Response interface {
	// Returns the kind of response as a string.
	Kind() string
}

// Represents a response to a handshake request.
type MagicResponse struct{}

func (MagicResponse) Kind() string { return MagicResponseKind }

// Represents a response to an add account request.
type AddAccountResponse struct {
	AccountID uint64
}

func (AddAccountResponse) Kind() string { return AddAccountResponseKind }

// Represents a response to a get account request.
type GetAccountResponse struct {
	Credits uint128.Uint128
	Debits  uint128.Uint128
	Flags   uint8
}

func (GetAccountResponse) Kind() string { return GetAccountResponseKind }

// Represents a response to a count database request.
type CountDatabaseResponse struct {
	Accounts     uint64
	Transactions uint64
}

func (CountDatabaseResponse) Kind() string { return CountDatabaseResponseKind }

// Represents a response to a successful operation.
type OkResponse struct{}

func (OkResponse) Kind() string { return OkResponseKind }

// Represents a response to an error condition.
type ErrorResponse struct {
	Code uint16
}

func (ErrorResponse) Kind() string { return ErrorResponseKind }

// Parses a byte slice into a `Response` object based on the response kind.
func ParseResponse(data []byte) (Response, error) {
	data = bytes.TrimSpace(data) // Remove any leading or trailing whitespace

	if len(data) == 0 {
		return nil, fmt.Errorf("empty response")
	}

	// Split the data into tokens based on spaces.
	parameters := bytes.Split(data, []byte(" "))

	// The first token is the kind of response.
	kind := string(parameters[0])

	// Determine the type of response.
	switch kind {
	case MagicResponseKind:
		if len(parameters) != 1 {
			return nil, fmt.Errorf("invalid magic response format")
		}

		return MagicResponse{}, nil
	case OkResponseKind:
		if len(parameters) != 1 {
			return nil, fmt.Errorf("invalid ok response format")
		}

		return OkResponse{}, nil
	case AddAccountResponseKind:
		if len(parameters) != 2 {
			return nil, fmt.Errorf("invalid add account response format")
		}

		accountID, err := strconv.ParseUint(string(parameters[1]), 16, 64)
		if err != nil {
			return nil, fmt.Errorf("failed to parse account id: %w", err)
		}

		return AddAccountResponse{AccountID: accountID}, nil
	case GetAccountResponseKind:
		if len(parameters) != 4 {
			return nil, fmt.Errorf("invalid get account response format")
		}

		credits, err := uint128.Parse(string(parameters[1]), 16)
		if err != nil {
			return nil, fmt.Errorf("failed to parse credits: %w", err)
		}

		debits, err := uint128.Parse(string(parameters[2]), 16)
		if err != nil {
			return nil, fmt.Errorf("failed to parse debits: %w", err)
		}

		flags, err := strconv.ParseUint(string(parameters[3]), 16, 8)
		if err != nil {
			return nil, fmt.Errorf("failed to parse flags: %w", err)
		}

		return GetAccountResponse{Credits: credits, Debits: debits, Flags: uint8(flags)}, nil
	case CountDatabaseResponseKind:
		if len(parameters) != 3 {
			return nil, fmt.Errorf("invalid count database response format")
		}

		accounts, err := strconv.ParseUint(string(parameters[1]), 16, 64)
		if err != nil {
			return nil, fmt.Errorf("failed to parse accounts: %w", err)
		}

		transactions, err := strconv.ParseUint(string(parameters[2]), 16, 64)
		if err != nil {
			return nil, fmt.Errorf("failed to parse transactions: %w", err)
		}

		return CountDatabaseResponse{Accounts: accounts, Transactions: transactions}, nil
	case ErrorResponseKind:
		if len(parameters) != 2 {
			return nil, fmt.Errorf("invalid error response format")
		}

		code, err := strconv.ParseUint(string(parameters[1]), 16, 16)
		if err != nil {
			return nil, fmt.Errorf("failed to parse error code: %w", err)
		}

		return ErrorResponse{Code: uint16(code)}, nil
	default:
		return nil, fmt.Errorf("unknown response type received: %s", kind)
	}
}
