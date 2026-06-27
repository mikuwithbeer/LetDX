package http

import "fmt"

const (
	FLAG_NONE        uint8 = 0
	FLAG_CAN_SEND    uint8 = 1 << 0 // 00000001
	FLAG_CAN_RECEIVE uint8 = 1 << 1 // 00000010
	FLAG_CAN_DEBT    uint8 = 1 << 2 // 00000100
)

// Flags represents a set of permissions for an account.
type Flags []string

// Converts a Flags slice to its corresponding uint8 representation.
func (f Flags) ToUint8() (uint8, error) {
	result := uint8(FLAG_NONE)

	for _, flag := range f {
		switch flag {
		case "send":
			result |= FLAG_CAN_SEND
		case "receive":
			result |= FLAG_CAN_RECEIVE
		case "debt":
			result |= FLAG_CAN_DEBT
		default:
			return result, fmt.Errorf("invalid flag: %s", flag)
		}
	}

	return result, nil
}

// Converts a uint8 representation of flags to a Flags slice.
func Uint8ToFlags(flags uint8) Flags {
	result := Flags{}

	if flags&FLAG_CAN_SEND != 0 {
		result = append(result, "send")
	}

	if flags&FLAG_CAN_RECEIVE != 0 {
		result = append(result, "receive")
	}

	if flags&FLAG_CAN_DEBT != 0 {
		result = append(result, "debt")
	}

	return result
}
