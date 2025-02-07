package config

import (
	"fmt"
	"os"
	"path/filepath"

	"gopkg.in/yaml.v3"
)

type Configs struct {
	Redis struct {
		PAddr   []string `yaml:"addrs"`
		AppName string   `yaml:"app_name"`
	} `yaml:"redis"`

	MySQLDB struct {
		Addr     string `yaml:"addr"`
		DBName   string `yaml:"db_name"`
		Account  string `yaml:"account"`
		Password string `yaml:"password"`
	} `yaml:"mysqldb"`
}

func ReadConfig(conf interface{}, path string) (interface{}, error) {
	filename, _ := filepath.Abs(path)
	f, err := os.ReadFile(filename)
	if err != nil {
		fmt.Println(err)
		return nil, err
	}

	err = yaml.Unmarshal(f, conf)
	if err != nil {
		fmt.Println(err)
		return nil, err
	}
	return conf, nil
}

func InitConfig() {

}
