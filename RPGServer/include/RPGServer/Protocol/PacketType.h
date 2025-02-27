#pragma once

/*
* Header 에서 2byte 사용 중
* 
* 
*/
enum EPacketType {

	CS_CHAT = 10,
	SC_CHAT = 11,


	/*
	
	position
	x - 4 byte
	y - 4 byte
	z - 4 byte

	rotation
	yaw - 2 byte
	pitch - 2 byte
	roll - 2 byte

	speed - velocity
	vx - 2 byte
	vy - 2 byte
	vz - 2 byte

	*/
	CS_MOVE_MY = 20,

	CS_ECHO = 1000,
	SC_ECHO = 1001
};