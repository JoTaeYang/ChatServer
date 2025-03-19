#include "RPGServer/User/User.h"

#include <iostream>

void User::Init()
{	
	memset(sessionHex, 0, 16);
	updatePos(0,0,0,0,0,0,0,0,0);

	Status = User::Status::GAME;

	sessionKey = "";
}

void User::UpdateCharacterMove(float PX, float PY, float PZ, float RX, float RY, float RZ, float VX, float VY, float VZ)
{
	updatePos(PX, PY, PZ, RX, RY, RZ, VX, VY, VZ);
		
}

void User::InitPlayer(float PX, float PY, float PZ, float RX, float RY, float RZ, float VX, float VY, float VZ)
{
	updatePos(PX, PY, PZ, RX, RY, RZ, VX, VY, VZ);
}


void User::GetCreatePosRot(float& PX, float& PY, float& PZ, float& RX, float& RY, float& RZ)
{
	PX = PosX;
	PY = PosY;	
	PZ = PosZ;

	RX = RotX;
	RY = RotY;
	RZ = RotZ;
}

void User::GetMoveInfos(float& PX, float& PY, float& PZ, float& RX, float& RY, float& RZ, float& VX, float& VY, float& VZ)
{
	PX = PosX;
	PY = PosY;
	PZ = PosZ;

	RX = RotX;
	RY = RotY;
	RZ = RotZ;

	VX = this->VX;
	VY = this->VY;
	VZ = this->VZ;
}

void User::updatePos(float PX, float PY, float PZ, float RX, float RY, float RZ, float VX, float VY, float VZ)
{
	PosX = PX;
	PosY = PY;
	PosZ = PZ;

	RotX = RX;
	RotY = RY;
	RotZ = RZ;

	this->VX = VX;
	this->VY = VY;
	this->VZ = VZ;
}
