package base

import (
	"strings"

	"github.com/google/uuid"
)

func GenUUID() string {
	u := uuid.New()
	return strings.ReplaceAll(u.String(), "-", "")
}
