package uint128

import (
	"errors"
	"math/bits"
	"strconv"
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

func Parse(hexadecimal string) (Uint128, error) {
	length := len(hexadecimal)

	if length == 0 {
		return Uint128{}, errors.New("empty string")
	}

	if length > 32 {
		return Uint128{}, errors.New("hexadecimal too long")
	}

	uint128 := Uint128{}
	err := error(nil)

	if length <= 16 {
		uint128.Low, err = strconv.ParseUint(hexadecimal, 16, 64)
		return uint128, err
	}

	low := hexadecimal[length-16:]
	high := hexadecimal[:length-16]

	uint128.Low, err = strconv.ParseUint(low, 16, 64)
	if err != nil {
		return uint128, err
	}

	uint128.High, err = strconv.ParseUint(high, 16, 64)
	return uint128, err
}
