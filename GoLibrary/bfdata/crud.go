package bfdata

import (
	"github.com/JoTaeYang/ChatServer/GoLibrary/bfdata/rdb"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bfsql"
)

func SelectFromIdentity[T any](pk string) error {
	db := bfsql.RDB.GetIdentityDB()

	query := bfsql.MakeIdentifierSelectQuery()

	rows := db.QueryRow(query, pk)

	var data interface{} = new(T)

	err := data.(rdb.IModel).Init(rows)
	if err != nil {
		return err
	}
	return nil
}
