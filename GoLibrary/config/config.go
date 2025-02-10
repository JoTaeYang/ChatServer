package config

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/JoTaeYang/ChatServer/GoLibrary/bfsql"
	"gopkg.in/yaml.v3"
)

type Configs struct {
	// Redis struct {
	// 	PAddr   []string `yaml:"addrs"`
	// 	AppName string   `yaml:"app_name"`
	// } `yaml:"redis,omitempty"`

	MySQLDB []bfsql.Config `yaml:"mysqldb,omitempty"`

	Server struct {
		Port string `yaml:"port"`
	} `yaml:"setting"`
}

func readConfig(conf *Configs, path string) error {
	filename, _ := filepath.Abs(path)
	f, err := os.ReadFile(filename)
	if err != nil {
		fmt.Println(err)
		return err
	}

	err = yaml.Unmarshal(f, &conf)
	if err != nil {
		fmt.Println(err)
		return err
	}
	return nil
}

func InitConfig(conf *Configs, path string) error {
	var err error
	err = readConfig(conf, path)
	if err != nil {
		return err
	}

	if len(conf.MySQLDB) > 0 {
		bfsql.InitService(conf.MySQLDB)
	}

	return nil
}
