package api

import (
	"log"
	"net/http"
	"time"

	"github.com/JoTaeYang/ChatServer/GoLibrary/base"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bf"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bfdata"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bfredis"
	"github.com/gin-gonic/gin"
)

type LoginApi struct {
	base.Api

	auth       bfdata.Auth
	identifier bfdata.Identifier
}

func Login(c *gin.Context) {
	r := &LoginApi{}

	if !r.Check(c) {
		return
	}

	if !r.Load(c) {
		return
	}

	if !r.Exec(c) {
		return
	}

	if !r.DbSave(c) {
		return
	}

	if !r.CacheSave(c) {
		return
	}
}

func (r *LoginApi) Check(c *gin.Context) bool {
	var err error
	req := &bf.LoginReq{}
	err = base.UnMarshal(c.Request.Body, req)
	if err != nil {
		return false
	}

	log.Println(req)

	if req.Id == "" || req.Pw == "" {
		c.JSON(http.StatusOK, gin.H{
			"err_code": 1,
		})
		return false
	}

	r.Req = req
	return true
}

func (r *LoginApi) Load(c *gin.Context) bool {
	var err error
	req := r.Req.(*bf.LoginReq)

	identityRepo := bfdata.IdentiferRepository{}
	err = identityRepo.Select(&r.identifier, req.Id)
	if err != nil {
		return false
	}

	authRepo := bfdata.AuthRepository{}
	err = authRepo.SelectId(&r.auth, req.Id, r.identifier.ShardIdx)
	if err != nil {
		return false
	}

	r.Uid = r.auth.PK
	return true
}

func (r *LoginApi) Exec(c *gin.Context) bool {
	bfredis.DelSessionKey(c.Request.Context(), r.auth.SessionKey)

	r.auth.SessionKey = base.GenUUID()
	r.auth.UpdateAt = time.Now().UTC().Unix()
	return true
}

func (r *LoginApi) DbSave(c *gin.Context) bool {
	var err error
	authRepo := bfdata.AuthRepository{}

	updateData := map[string]interface{}{
		"session_key": r.auth.SessionKey,
		"update_at":   r.auth.UpdateAt,
	}

	log.Println(r.auth.SessionKey)

	err = authRepo.Update(&r.auth, r.Uid, r.identifier.ShardIdx, updateData)
	if err != nil {
		return false
	}

	return true
}

func (r *LoginApi) CacheSave(c *gin.Context) bool {
	var err error
	err = bfredis.SetSessionKey(c.Request.Context(), r.Uid, r.SessionKey)
	if err != nil {
		return false
	}
	return true
}

func (r *LoginApi) Ans(c *gin.Context) bool {
	ans := bf.LoginAns{
		Key: r.SessionKey,
	}
	c.JSON(http.StatusOK, &ans)
	return true
}

func (r *LoginApi) Logging(c *gin.Context) bool {

	return true
}
