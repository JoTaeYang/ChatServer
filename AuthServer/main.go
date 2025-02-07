package main

import (
	"github.com/JoTaeYang/ChatServer/tree/main/AuthServer/api"
	"github.com/gin-gonic/gin"
)

var (
	engine *gin.Engine
)

func CORSM() gin.HandlerFunc {
	return func(c *gin.Context) {
		c.Header("Access-Control-Allow-Headers", "Content-Type, Authorization, Origin")
		c.Header("Access-Control-Allow-Origin", "*")
		c.Header("Access-Control-Allow-Method", "GET, DELETE, POST")
		c.Header("Access-Control-Allow-Credentials", "true")

		if c.Request.Method == "OPTIONS" {
			c.AbortWithStatus(204)
			return
		}
	}
}

func InitRouter() *gin.Engine {
	r := gin.Default()

	r.Use(CORSM())

	AuthRouter := r.Group("/big/auth")
	{
		AuthRouter.POST("/signup", api.SignUp)
	}

	return r
}

func main() {

	engine = InitRouter()

	engine.Run()
}
