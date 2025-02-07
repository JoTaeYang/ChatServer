package api

import (
	"net/http"

	"github.com/gin-gonic/gin"
)

type LoginApi struct {
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

	c.JSON(http.StatusOK, nil)
	return true
}

func (r *LoginApi) Logging(c *gin.Context) bool {
	return true
}
