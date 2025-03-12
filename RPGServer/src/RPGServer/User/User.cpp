#include "RPGServer/User/User.h"

void User::Init()
{
	updatePos(0,0,0,0,0,0,0,0,0);

	Status = User::Status::GAME;
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

void User::updatePos(float PX, float PY, float PZ, float RX, float RY, float RZ, float VX, float VY, float VZ)
{
	PosX = PX;
	PosY = PY;
	PosZ = PZ;

	RotX = RX;
	RotY = RY;
	RotZ = RZ;

	VX = VX;
	VY = VY;
	VZ = VZ;
}
