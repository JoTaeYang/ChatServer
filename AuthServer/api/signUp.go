package api

import (
	"net/http"

	"github.com/gin-gonic/gin"
)

type SignUpApi struct {
}

func SignUp(c *gin.Context) {

}

func (r *SignUpApi) Check(c *gin.Context) bool {

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

	c.JSON(http.StatusOK, nil)
	return true
}

func (r *SignUpApi) Logging(c *gin.Context) bool {
	return true
}
