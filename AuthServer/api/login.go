package api

import (
	"net/http"

	"github.com/JoTaeYang/ChatServer/GoLibrary/base"
	"github.com/JoTaeYang/ChatServer/GoLibrary/bf"
	"github.com/gin-gonic/gin"
)

type LoginApi struct {
	base.Api
}

func Login(c *gin.Context) {

}

func (r *LoginApi) Check(c *gin.Context) bool {

	return true
}

func (r *LoginApi) Load(c *gin.Context) bool {

	return true
}

func (r *LoginApi) Exec(c *gin.Context) bool {

	return true
}

func (r *LoginApi) DbSave(c *gin.Context) bool {

	return true
}

func (r *LoginApi) CacheSave(c *gin.Context) bool {

	return true
}

func (r *LoginApi) Ans(c *gin.Context) bool {
	ans := bf.LoginAns{}
	c.JSON(http.StatusOK, &ans)
	return true
}

func (r *LoginApi) Logging(c *gin.Context) bool {
	return true
}
