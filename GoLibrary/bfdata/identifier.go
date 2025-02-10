package bfdata

import (
	"database/sql"

	"github.com/JoTaeYang/ChatServer/GoLibrary/bfdata/rdb"
)

type Identifier struct {
	rdb.RdbPK[Identifier]

	ShardIdx      int32
	InstanceIndex int32
}

func (d *Identifier) GetDBName() string {
	return "IDENTIFIER"
}

func (d *Identifier) GetRDBTable() string {
	return "bfidentify"
}

func (d *Identifier) Init(rows *sql.Row) (err error) {
	err = rows.Scan(&d.PK, &d.ShardIdx, &d.InstanceIndex)
	return
}
