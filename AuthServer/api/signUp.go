package api

import (
	"log"
	"net/http"

	"github.com/JoTaeYang/ChatServer/GoLibrary/base"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bf"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bfdata"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bfsql"
	"github.com/gin-gonic/gin"
)

type SignUpApi struct {
	base.Api

	sessionKey string
}

func SignUp(c *gin.Context) {
	r := &SignUpApi{}

	if !r.Check(c) {
		return
	}

	if !r.Load(c) {
		return
	}

	if !r.DbSave(c) {
		return
	}
}

func (r *SignUpApi) Check(c *gin.Context) bool {
	var err error
	req := &bf.SignUpReq{}
	err = base.UnMarshal(c.Request.Body, req)
	if err != nil {
		return false
	}

	log.Println(req)

	if req.Id == "" {
		c.JSON(http.StatusOK, gin.H{
			"err_code": 1,
		})
		return false
	}

	if req.Pw != req.Pw_Check {
		c.JSON(http.StatusOK, gin.H{
			"err_code": 1,
		})
		return false
	}

	r.Req = req
	return true
}

func (r *SignUpApi) Load(c *gin.Context) bool {
	var err error
	req := r.Req.(*bf.SignUpReq)
	// 이미 있다는 것
	if err == bfdata.SelectFromIdentity[bfdata.Identifier](req.Id) {
		c.JSON(http.StatusOK, gin.H{
			"err_code": 1,
		})
		return false
	}
	return true
}

func (r *SignUpApi) DbSave(c *gin.Context) bool {
	req := r.Req.(*bf.SignUpReq)
	repo := bfdata.IdentiferRepository{}
	autoRepo := bfdata.AuthRepository{}

	data := &bfdata.Identifier{
		ShardIdx: bfsql.RDB.GetGameShardIndex(),
	}

	data.SetPK(req.Id)

	err := repo.Insert(data)
	if err != nil {
		c.JSON(http.StatusOK, gin.H{
			"err_code": 1,
		})
		return false
	}

	auth := &bfdata.Auth{
		Id:         req.Id,
		Pw:         req.Pw,
		SessionKey: base.GenUUID(),
		Attr: map[string]string{
			"DUMMY": "DUMMY",
		},
	}

	auth.SetPK(base.GenUUID())
	err = autoRepo.Insert(auth, data.ShardIdx)
	if err != nil {
		repo.Delete(data)
		c.JSON(http.StatusOK, gin.H{
			"err_code": 1,
		})
		return false
	}

	return true
}

func (r *SignUpApi) CacheSave(c *gin.Context) bool {

	return true
}

func (r *SignUpApi) Ans(c *gin.Context) bool {
	ans := bf.SignUpAns{}
	c.JSON(http.StatusOK, &ans)
	return true
}

func (r *SignUpApi) Logging(c *gin.Context) bool {
	return true
}
