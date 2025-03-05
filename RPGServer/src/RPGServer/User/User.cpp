#include "RPGServer/User/User.h"

void User::Init()
{
	PosX = 0;
	PosY = 0;
	PosZ = 0;

	RotX = 0;
	RotY = 0;
	RotZ = 0;

	VX = 0;
	VY = 0;
	VZ = 0;
}

void User::UpdateCharacterMove(float PX, float PY, float PZ, float RX, float RY, float RZ, float VX, float VY, float VZ)
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
