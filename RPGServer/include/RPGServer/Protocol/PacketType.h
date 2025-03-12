#pragma once

/*
* Header 에서 2byte 사용 중
* 
* 
*/
enum EPacketType {

	/*
	position
	x - 4
	y - 4
	z - 4

	rotation
	yaw - 4
	pitch - 4
	roll - 4
	*/
	SC_CREATE_MY = 10,

	/*
	position
	x - 4
	y - 4
	z - 4

	rotation
	yaw - 4
	pitch - 4
	roll - 4
	*/
	SC_CREATE_OTHER = 11,

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