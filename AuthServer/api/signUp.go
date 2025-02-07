package api

import (
	"log"
	"net/http"
	"reflect"
	"time"

	"github.com/gin-gonic/gin"
)

type SignUpApi struct {
}

func SignUp(c *gin.Context) {

}

func (r *SignUpApi) Check(c *gin.Context) bool {
	r.Req = &battleidle.SignUpReq{}
	err := base.UnMarshal(c.Request.Body, r.Req, r.Uid)
	if err != nil {
		return false
	}

	req := r.Req.(*battleidle.SignUpReq)
	if req.Uid == "" {
		c.Error(middleware.NewApiErrorCustom(r.Uid, "not recv uid data", 1, reflect.TypeOf(r).Name()))
		return false
	}
	r.Uid = req.Uid

	return true
}

func (r *SignUpApi) Load(c *gin.Context) bool {
	var err error
	req := r.Req.(*battleidle.SignUpReq)

	r.auth, err = dynamodb.GetAuth(c.Request.Context(), req.Uid)
	if err != nil {
		return false
	}

	r.prevAuth = &battleidle.Auth{
		PK: r.auth.PK,
	}
	return true
}

func (r *SignUpApi) Exec(c *gin.Context) bool {
	req := r.Req.(*battleidle.SignUpReq)
	if r.auth.PK == "" {
		var channel dynamodb.ChannelResult
		channelInfo := make(chan dynamodb.ChannelResult)
		defer close(channelInfo)
		dynamodb.EnqChannelInfoJob(channelInfo)

		tick := time.Tick(time.Second * 5)

		select {
		case <-tick:
			log.Print("channel info get error!!")
			break
		case channel = <-channelInfo:
			log.Print("channel info get success")
			break
		}

		if channel.Channel == "" || channel.ErrStr != nil {
			return false
		}

		r.SessionKey = core.GenUUID()
		r.auth = &battleidle.Auth{
			PK:  req.Uid,
			SK:  channel.Channel,
			Key: r.SessionKey,
		}
	} else {
		_ = rediscache.DelSessionKey(c.Request.Context(), r.auth.Key)
		r.SessionKey = core.GenUUID()
		r.auth.Key = r.SessionKey
	}
	return true
}

func (r *SignUpApi) DbSave(c *gin.Context) bool {
	var err error

	return true
}

func (r *SignUpApi) CacheSave(c *gin.Context) bool {
	var err error

	return true
}

func (r *SignUpApi) Ans(c *gin.Context) bool {

	c.JSON(http.StatusOK, nil)
	return true
}

func (r *SignUpApi) Logging(c *gin.Context) bool {
	return true
}
