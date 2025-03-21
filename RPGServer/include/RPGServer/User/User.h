#pragma once


#include <string>

class User
{
public:
	enum Status
	{
		NONE = 0,

		GAME
	};
public:
	User() = default;	

	void Init();

	void UpdateCharacterMove(float PX, float PY, float PZ,
		float RX, float RY, float RZ,
		float VX, float VY, float VZ);

	void InitPlayer(float PX, float PY, float PZ,
		float RX, float RY, float RZ,
		float VX, float VY, float VZ);

	bool IsGame() {
		return Status == GAME;
	}

public:
	void GetCreatePosRot(float& PX, float& PY, float& PZ,
		float& RX, float& RY, float& RZ);

	void GetMoveInfos(float& PX, float& PY, float& PZ,
		float& RX, float& RY, float& RZ,
		float& VX, float& VY, float& VZ);

	void Leave();

	std::string sessionKey;
	char sessionHex[16];

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

	Status Status;

};
