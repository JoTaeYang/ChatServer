package bfredis

import (
	"context"
	"strings"
	"time"

	jsoniter "github.com/json-iterator/go"
)

var (
	sessionTTL time.Duration = time.Hour * 1
)

type SessionInfo struct {
	Uid      string
	ShardIdx int32
}

func DelSessionKey(ctx context.Context, sessionKey string) error {
	stringArr := []string{
		appName,
		sessionKey,
		"session",
	}

	key := strings.Join(stringArr, ":")

	_, err := Redis.WriteRedisCli.Del(ctx, key).Result()
	if err != nil {
		return err
	}

	return nil
}

func SetSessionKey(ctx context.Context, uid, sessionKey string) error {
	stringArr := []string{
		appName,
		sessionKey,
		"session",
	}

	key := strings.Join(stringArr, ":")

	info := &SessionInfo{
		Uid: uid,
	}

	var json = jsoniter.ConfigCompatibleWithStandardLibrary
	infoJson, _ := json.Marshal(info)

	_, err := Redis.WriteRedisCli.Set(ctx, key, infoJson, sessionTTL).Result()
	if err != nil {
		return err
	}

	return nil
}
