package bfsql

import "strings"

func MakeIdentifierSelectQuery() string {
	queries := []string{
		`SELECT * FROM`,
		IdentifierTable,
		`where pk = ?`,
	}

	resultQuery := strings.Join(queries, " ")

	return resultQuery
}

func MakeIdentifierInsertQuery() string {
	queries := []string{
		`INSERT INTO`,
		IdentifierTable,
		`VALUES (?, ?, ?, ?, ?)`,
	}

	return strings.Join(queries, " ")
}
