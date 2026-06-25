package uint128

import (
	"bytes"
	"errors"
	"fmt"
	"math/big"
	"math/bits"
)

// Used for highly optimized base-10 string conversion.
const QUINTILLION = 1e18

// Represents a 128-bit unsigned integer.
type Uint128 struct {
	High uint64
	Low  uint64
}

// Returns the byte slice representation of the 128-bit unsigned integer.
func (u Uint128) Bytes() []byte {
	if u.High == 0 && u.Low == 0 {
		return []byte("0")
	}

	var buffer [40]byte // 40 bytes is strictly enough to hold the maximum 128-bit unsigned integer
	index := len(buffer)
	length := index

	for u.High != 0 || u.Low != 0 {
		// Divide the 128-bit number (u.High, u.Low) by 10^18.
		high, remHigh := bits.Div64(0, u.High, QUINTILLION)
		low, rem := bits.Div64(remHigh, u.Low, QUINTILLION)

		u.High = high
		u.Low = low

		// Extract base-10 digits from the remainder.
		for range 18 {
			index--
			buffer[index] = '0' + byte(rem%10)
			rem /= 10

			// If the remaining number is fully processed and the chunk has no more digits, stop padding with zeros.
			if u.High == 0 && u.Low == 0 && rem == 0 {
				break
			}
		}
	}

	// Copy the relevant portion of the buffer to the result slice.
	result := make([]byte, length-index)
	copy(result, buffer[index:])

	return result
}

// Returns the string representation of the 128-bit unsigned integer.
func (u Uint128) String() string {
	return string(u.Bytes())
}

// Parses a string representation of a number in the specified base into a `Uint128`.
func Parse(input string, base int) (Uint128, error) {
	if len(input) == 0 {
		return Uint128{}, errors.New("empty string")
	}

	number, ok := new(big.Int).SetString(input, base)
	if !ok {
		return Uint128{}, errors.New("invalid string format for the given base")
	}

	uint128Max := new(big.Int).Sub(new(big.Int).Lsh(big.NewInt(1), 128), big.NewInt(1))
	if number.Cmp(uint128Max) > 0 {
		return Uint128{}, errors.New("value exceeds 128 bits")
	}

	mask := new(big.Int).SetUint64(^uint64(0))
	return Uint128{
		High: new(big.Int).Rsh(number, 64).Uint64(),
		Low:  new(big.Int).And(number, mask).Uint64(),
	}, nil
}

// Implements the `json.Marshaler` interface.
func (u Uint128) MarshalJSON() ([]byte, error) {
	return fmt.Appendf(nil, "%q", u.Bytes()), nil
}

// Implements the `json.Unmarshaler` interface.
func (u *Uint128) UnmarshalJSON(b []byte) error {
	// Safely ignore standard JSON null values.
	if bytes.Equal(b, []byte("null")) {
		return nil
	}

	// Strip surrounding double quotes for string-encoded numbers.
	b = bytes.TrimPrefix(b, []byte(`"`))
	b = bytes.TrimSuffix(b, []byte(`"`))

	// Base 10 is assumed for JSON representations.
	result, err := Parse(string(b), 10)
	if err != nil {
		return err
	}

	*u = result
	return nil
}
