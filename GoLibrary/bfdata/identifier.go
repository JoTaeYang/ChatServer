package bfdata

import (
	"database/sql"
	"errors"
	"strings"

	"github.com/JoTaeYang/ChatServer/GoLibrary/bfdata/rdb"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bfsql"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bftime"
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
	return "bigidentity"
}

func (d *Identifier) Init(rows *sql.Row) (err error) {
	err = rows.Scan(&d.PK, &d.ShardIdx, &d.UpdateAt, &d.CreateAt, &d.ExpireAt)
	//err = rows.Scan(&d)
	return
}

type IdentiferRepository struct {
}

func (i *IdentiferRepository) Insert(data *Identifier) error {
	db := bfsql.RDB.GetIdentityDB()
	if db == nil {
		return errors.New("db object nil")
	}

	now := bftime.Now().Unix()
	data.CreateAt = now
	data.UpdateAt = now
	data.ExpireAt = now + 36000

	query := bfsql.MakeIdentifierInsertQuery()

	result, err := db.Exec(query,
		data.PK, data.ShardIdx, data.UpdateAt, data.CreateAt, data.ExpireAt)
	if err != nil {
		return err
	}

	_, err = result.RowsAffected()
	if err != nil {
		return err
	}
	return nil
}

func (i *IdentiferRepository) Delete(data *Identifier) error {
	db := bfsql.RDB.GetIdentityDB()
	if db == nil {
		return errors.New("db object nil")
	}
	tableNameRDB := data.GetRDBTable()
	queries := []string{
		`DELETE FROM`,
		tableNameRDB,
		`WHERE pk = ?`,
	}

	query := strings.Join(queries, " ")

	result, err := db.Exec(query, data.PK)
	if err != nil {
		return err
	}

	_, err = result.RowsAffected()
	if err != nil {
		return err
	}
	return nil
}
