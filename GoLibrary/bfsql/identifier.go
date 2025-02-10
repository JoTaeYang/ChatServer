package bfsql

import "strings"

var IdentifierTable string = "bigIdentity"

func MakeIdentifierSelectQuery() string {
	queries := []string{
		`SELECT * FROM `,
		IdentifierTable,
		`where pk = ?`,
	}

	resultQuery := strings.Join(queries, "")

	return resultQuery
}
