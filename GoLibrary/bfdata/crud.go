package bfdata

import (
	"database/sql"

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

// SelectFromId를 하려고 보니까 Identity 데이터가 필요해짐
// 근데 Identity는 select를 저렇게 밖에 안함.
// 그럼 밖에서 매개변수를 받아서 한다면?
// 좀 Generic 하게 만들 수 있지 않을까? 라는 생각을 했다
// 근데 먼저 Identity를 하고 Auth에 Select를 한다면,, 음.. 이게 인덱스가 필요한가? 는 고민이 좀 필요할듯하다

func SelectFromPk[T any](db *sql.DB, query, pk string, data T) error {
	rows := db.QueryRow(query, pk)

	var dataInterface interface{} = data
	return dataInterface.(rdb.IModel).Init(rows)
}

func SelectFromPK[T any](db *sql.DB, query, pk string) (*T, error) {
	rows := db.QueryRow(query, pk)

	data := new(T)

	var dataInterface interface{} = data

	err := dataInterface.(rdb.IModel).Init(rows)
	if err != nil {
		return nil, err
	}
	return data, nil
}

// Data Load 할 때 PK 만 쓰는 애랑
// 그 외랑 해서 Generic 하게 만들 수 있을 거 같기도 한데
