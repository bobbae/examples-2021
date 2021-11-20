package dnacodec

import (
	"crypto/md5"
	"encoding/hex"
	"fmt"
	"hash/crc32"
	"io/ioutil"
	"math/rand"
	//"gitlab.com/bobbae/q"
)

var crc32q = crc32.MakeTable(0xD5828281)

const letterBytes = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

func GetCRC32(text string) string {
	//q.Q("get crc32 for", text)
	return fmt.Sprintf("%08x", crc32.Checksum([]byte(text), crc32q))
}

func GetMD5Hash(text string) string {
	hasher := md5.New()
	hasher.Write([]byte(text))
	hash := hex.EncodeToString(hasher.Sum(nil))
	//q.Q("get md5 for", text, hash)
	return hash
}

var numBytes = 0

func Encode(n int, line string) string {
	l := ""
	//q.Q("encode len", n, "line", line)
	for i := 0; i < n; i++ {
		b := EncodeByte(byte(line[i]))
		l = l + b
		numBytes++
	}
	return l
}

var dnaLetters = []string{"A", "C", "G", "T"}

func EncodeByte(b byte) string {
	s := ""
	for i := 0; i < 4; i++ {
		tb := b & 3
		s = s + dnaLetters[tb]
		b = b >> 2
	}
	return s
}

func Decode(n int, lineBuf string) string {
	l := ""
	for i := 0; i < n; i = i + 4 {
		if (i + 4) > n {
			//q.Q("unaligned")
			break
		}
		//q.Q("decode", i, n, lineBuf[i:i+4])
		b := DecodeByte(lineBuf[i : i+4])
		numBytes++
		l = l + b
	}
	//q.Q("decode return line", l)
	return l
}

var dnaVals = map[string]int{"A": 0, "C": 1, "G": 2, "T": 3}

func DecodeByte(b4 string) string {
	val := 0
	var i uint
	for i = 0; i < 4; i++ {
		//fmt.Printf("[%s]%d,", string(b4[i]), dnaVals[string(b4[i])])
		val = val | ((dnaVals[string(b4[i])] & 0x3) << (i << 1))
	}
	return string(val)
}

func PadBytes(dat []byte) {
	for i := range dat {
		dat[i] = letterBytes[rand.Intn(len(letterBytes))]
	}
}

func SplitFile(fn string, sz int) error {
	dat, err := ioutil.ReadFile(fn)
	if err != nil {
		return err
	}
	i := 0
	lim := 150
	for lim < len(dat) {
		if lim > len(dat) {
			prevlim := lim - 150
			lim = len(dat) - prevlim + prevlim
		}
		err = ioutil.WriteFile(fmt.Sprintf("fn-%d"), dat[i:lim], 0666)
		if err != nil {
			return err
		}
		lim += 150
		i += 150
	}
	return nil
}
