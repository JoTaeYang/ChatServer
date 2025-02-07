#pragma once

/*
* Header 에서 2byte 사용 중
* 
* 
*/
enum EPacketType {

	CS_CHAT = 10,
	SC_CHAT = 11,

	CS_MOVE_MY = 20,

	CS_ECHO = 1000,
	SC_ECHO = 1001
};