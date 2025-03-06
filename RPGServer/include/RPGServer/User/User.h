#pragma once

class User
{
public:
	User() = default;	

	void Init();

	void UpdateCharacterMove(float PX, float PY, float PZ,
		float RX, float RY, float RZ,
		float VX, float VY, float VZ);

	void InitPlayer(float PX, float PY, float PZ,
		float RX, float RY, float RZ,
		float VX, float VY, float VZ);

private:
	void updatePos(float PX, float PY, float PZ,
		float RX, float RY, float RZ,
		float VX, float VY, float VZ);

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
