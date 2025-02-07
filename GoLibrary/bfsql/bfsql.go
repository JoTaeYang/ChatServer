package bfsql

import (
	"database/sql"
	"fmt"

	_ "github.com/go-sql-driver/mysql"
)

type Config struct {
	Addr     string `yaml:"addr"`
	Port     string `yaml:"port"`
	DBName   string `yaml:"db_name"`
	Account  string `yaml:"account"`
	Password string `yaml:"password"`
}

func InitService(config Config) error {
	dataSourceName := fmt.Sprintf("%s:%s@tcp(%s:%s)/%s", config.Account, config.Password, config.Addr, config.Port, config.DBName)
	db, err := sql.Open("mysql", dataSourceName)
	if err != nil {
		return err
	}

	_ = db

	fmt.Println("mysql db service")
	return nil
}
