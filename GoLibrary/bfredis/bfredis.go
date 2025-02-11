package bfredis

import (
	"context"
	"errors"
	"log"

	redis "github.com/redis/go-redis/v9"
)

type Config struct {
	Addr    []string `yaml:"addrs"`
	AppName string   `yaml:"app_name"`
}

type RedisWrap struct {
	WriteRedisCli *redis.ClusterClient
	ReadRedisCli  *redis.ClusterClient
}

var (
	appname string
	Redis   RedisWrap
)

func InitService(cfg Config) error {
	appname = cfg.AppName

	Redis.WriteRedisCli = redis.NewClusterClient(&redis.ClusterOptions{
		Addrs:    cfg.Addr,
		PoolSize: 10,
	})
	if Redis.WriteRedisCli == nil {
		return errors.New("create redis read cli error")
	}

	Redis.ReadRedisCli = redis.NewClusterClient(&redis.ClusterOptions{
		Addrs:    cfg.Addr,
		PoolSize: 10,
	})
	if Redis.ReadRedisCli == nil {
		return errors.New("create redis write cli error")
	}
	log.Println(Redis.ReadRedisCli.Ping(context.Background()))

	return nil
}
