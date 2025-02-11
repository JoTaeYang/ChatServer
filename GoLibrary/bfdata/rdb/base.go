package rdb

import "database/sql"

type KeyType = int32

const (
	KeyTypeNone KeyType = iota
	KeyTypePK
)

type IModel interface {
	GetDBName() string
	GetRDBTable() string

	Init(*sql.Row) error
}
