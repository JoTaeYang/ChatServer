package bfsql

import (
	"database/sql"
	"fmt"

	_ "github.com/go-sql-driver/mysql"
)

/*
Exec() INSERT UPDATE DELETE 에서 사용
결과 값이 필요 없은 SQL 실행에 적합


QueryRow 단일행 조회
반드시 Scan을 통한 값 추출이 필요

Query() 여러 행을 조회
Next로 다음 데이터를 계속 조회
*/

type Config struct {
	Mode          string   `yaml:"mode"`
	Addr          []string `yaml:"addr"`
	Port          []string `yaml:"port"`
	DBName        string   `yaml:"db_name"`
	Account       string   `yaml:"account"`
	Password      string   `yaml:"password"`
	ShardCount    int32    `yaml:"shard_count"`
	InstanceCount int32    `yaml:"instance_count"`
}

type RDBWrap struct {
	dbs map[string][]*sql.DB
}

var (
	cfg map[string]Config
	RDB RDBWrap
)

func InitService(config []Config) error {
	RDB.dbs = make(map[string][]*sql.DB, len(config))
	cfg = make(map[string]Config, len(config))
	for _, v := range config {
		cfg[v.Mode] = v
		for j, _ := range v.Addr {
			dataSourceName := fmt.Sprintf("%s:%s@tcp(%s:%s)/%s", v.Account, v.Password, v.Addr[j], v.Port[j], v.DBName)

			db, err := sql.Open("mysql", dataSourceName)
			if err != nil {
				return err
			}

			fmt.Println("mysql db service : ", v.Port[j])

			if RDB.dbs[v.Mode] == nil {
				RDB.dbs[v.Mode] = make([]*sql.DB, len(v.Addr))
			}
			RDB.dbs[v.Mode][j] = db
		}
	}

	return nil
}

func (r *RDBWrap) GetIdentityDB() *sql.DB {
	key := "IDENTITY"
	return r.getDB(key, cfg[key].ShardCount)
}

func (r *RDBWrap) getDB(mode string, shard int32) *sql.DB {
	sIdx := int(shard - 1)

	if len(r.dbs[mode]) <= sIdx {
		return nil
	}

	return r.dbs[mode][sIdx]
}
