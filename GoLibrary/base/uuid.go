package base

import (
	"encoding/hex"
	"strings"

	"github.com/google/uuid"
)

func GenUUID() string {
	u1 := uuid.New()

	uuidStr := hex.EncodeToString(u1[:])
	return strings.ReplaceAll(uuidStr, "-", "")
}
