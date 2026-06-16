package uint128

import (
	"bytes"
	"errors"
	"fmt"
	"math/big"
	"math/bits"
)

const QUINTILLION = 1e18

type Uint128 struct {
	High uint64
	Low  uint64
}

func (u Uint128) Bytes() []byte {
	if u.High == 0 && u.Low == 0 {
		return []byte("0")
	}

	var buffer [40]byte
	index := len(buffer)
	length := index

	for u.High != 0 || u.Low != 0 {
		high, remHigh := bits.Div64(0, u.High, QUINTILLION)
		low, rem := bits.Div64(remHigh, u.Low, QUINTILLION)

		u.High = high
		u.Low = low

		for range 18 {
			index--
			buffer[index] = '0' + byte(rem%10)
			rem /= 10

			if u.High == 0 && u.Low == 0 && rem == 0 {
				break
			}
		}
	}

	result := make([]byte, length-index)
	copy(result, buffer[index:])

	return result
}

func (u Uint128) String() string {
	return string(u.Bytes())
}

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

func (u Uint128) MarshalJSON() ([]byte, error) {
	return fmt.Appendf(nil, "%q", u.Bytes()), nil
}

func (u *Uint128) UnmarshalJSON(b []byte) error {
	b = bytes.TrimPrefix(b, []byte(`"`))
	b = bytes.TrimSuffix(b, []byte(`"`))

	result, err := Parse(string(b), 10)
	if err != nil {
		return err
	}

	*u = result
	return nil
}
