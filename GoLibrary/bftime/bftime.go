package bftime

import (
	"log"
	"time"
)

var BaseTimePeriod int64 = 0

func Now() time.Time {
	return time.Now().Add(time.Duration(BaseTimePeriod * int64(time.Second)))
}

func SetBaseTimePeriod(period int64) {
	BaseTimePeriod = period
	log.Println("Base Time Add: ", period, Now())
}
