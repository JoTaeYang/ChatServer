package api

import (
	"net/http"

	"github.com/JoTaeYang/ChatServer/GoLibrary/base"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bf"
	"github.com/gin-gonic/gin"
)

type SignUpApi struct {
	base.Api
}

func SignUp(c *gin.Context) {
	r := &SignUpApi{}

	if !r.Check(c) {
		return
	}
}

func (r *SignUpApi) Check(c *gin.Context) bool {
	var err error
	r.Req = &bf.SignUpReq{}
	err = base.UnMarshal(c.Request.Body, r.Req)
	if err != nil {
		return false
	}

	return true
}

func (r *SignUpApi) Load(c *gin.Context) bool {

	return true
}

func (r *SignUpApi) Exec(c *gin.Context) bool {

	return true
}

func (r *SignUpApi) DbSave(c *gin.Context) bool {

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
