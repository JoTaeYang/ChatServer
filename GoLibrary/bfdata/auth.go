package bfdata

import (
	"database/sql"
	"encoding/json"
	"errors"
	"strings"

	"github.com/JoTaeYang/ChatServer/GoLibrary/bfdata/rdb"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bfsql"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bftime"
)

type Auth struct {
	rdb.RdbPK[Auth]

	Id         string
	Pw         string
	SessionKey string
	Attr       map[string]string
}

func (d *Auth) GetDBName() string {
	return "GAME"
}

func (d *Auth) GetRDBTable() string {
	return "bigauth"
}

func (d *Auth) Init(rows *sql.Row) (err error) {
	jsonStr := ""
	err = rows.Scan(&d.PK, &d.Id, &d.Pw, &d.SessionKey, &jsonStr, &d.UpdateAt, &d.CreateAt, &d.ExpireAt)

	return
}

type AuthRepository struct {
}

func (i *AuthRepository) Insert(data *Auth, shardIdx int32) error {
	db := bfsql.RDB.GetGameDB(shardIdx)
	if db == nil {
		return errors.New("db object nil")
	}

	now := bftime.Now().Unix()
	data.CreateAt = now
	data.UpdateAt = now
	data.ExpireAt = now + 36000

	RDBtable := data.GetRDBTable()

	queries := []string{
		`INSERT INTO`,
		RDBtable,
		`VALUES (?, ?, ?, ?, ?, ?, ?, ?)`,
	}

	query := strings.Join(queries, " ")

	jsonAttr, err := json.Marshal(data.Attr)
	if err != nil {
		return err
	}

	result, err := db.Exec(query,
		data.PK, data.Id, data.Pw, data.SessionKey, jsonAttr,
		data.UpdateAt, data.CreateAt, data.ExpireAt)
	if err != nil {
		return err
	}

	_, err = result.RowsAffected()
	if err != nil {
		return err
	}
	return nil
}
