#pragma once

class User
{
public:
	User() = default;	

	void Init();
private:
	float PosX;
	float PosY;
	float PosZ;

	float RotX;
	float RotY;
	float RotZ;
};
