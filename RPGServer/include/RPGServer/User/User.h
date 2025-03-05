#pragma once

class User
{
public:
	User() = default;	

	void Init();

	void UpdateCharacterMove(float PX, float PY, float PZ,
		float RX, float RY, float RZ,
		float VX, float VY, float VZ);
private:
	float PosX;
	float PosY;
	float PosZ;

	float RotX;
	float RotY;
	float RotZ;

	float VX;
	float VY;
	float VZ;
};
