package dnacodec

import (
	"testing"

	"gitlab.com/bobbae/q"
)

func TestPadBytes(t *testing.T) {
	dat := make([]byte, 150)
	PadBytes(dat)
	q.Level = "all"
	q.Output = "stdout"
	q.Q(string(dat))
}
