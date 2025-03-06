#include "RPGServer/Server/RPGServer.h"

#include <iostream>
#include <random>

#include "Library/Packet/Header.h"
#include "Library/MessageBuffer/MessageBuffer.h"
#include "Library/Profile/Profile.h"

#include "RPGServer/User/User.h"
#include "RPGServer/Protocol/PacketType.h"



bool RPGServer::Start(const int InSessionCount, const CSetting& InSetting)
{
	CServer::Start(InSessionCount, InSetting);

	users = new User[InSessionCount];

	//users.reserve(InSessionCount);
	//for (int i = 0; i < InSessionCount; ++i)
	//{
	//	auto tmpUser = std::make_unique<User>();
	//	users.emplace_back(std::move(tmpUser));
	//}

	
	return true;
}

/*
세션을 활성화 시킨다.
*/
void RPGServer::OnJoin(int Index)
{
	users[Index].Init();

	CMessageBuffer* pBuffer = CMessageBuffer::Alloc();

	CreatePlayer(Index, pBuffer);

	SendPacket(Index, pBuffer);

	pBuffer->DecRef();
}

void RPGServer::OnRecv(int Index, class CMessageBuffer* Buffer)
{
	Header header;

	Buffer->GetData((char*)&header, sizeof(Header));

	if (header.byCode != 0x89)
	{
		// TODO :: 소켓 연결 종료
	}

	switch (header.wType)
	{
	case EPacketType::CS_MOVE_MY:
		PacketProc_Move_Client(Index, Buffer);
		break;
	}
}

void RPGServer::CreatePlayer(int Index, CMessageBuffer* buffer)
{
	Header header;

	float posX, posY = 0.0f, posZ;
	float yaw = 0.1f, pitch = 0.1f, roll = 0.1f;

	std::random_device t;
	std::mt19937 gen(t());
	std::uniform_int_distribution<int> dis(0, 2000);
		
	header.byCode = 0x89;
	header.wType = EPacketType::SC_CREATE_MY;
	header.bySize;

	posX = dis(gen);
	posZ = dis(gen);

	buffer->PutData((char*)&header, sizeof(Header));

	this->users[Index].InitPlayer(posX, posY, posZ, yaw, pitch, roll, 0, 0, 0);

	*buffer << posX << posY << posZ << yaw << pitch << roll;
}

void RPGServer::PacketProc_Move_Client(int Index, CMessageBuffer* Buffer)
{
	float x, y, z;
	short yaw, pitch, roll;
	short vx, vy, vz;

	Profile& profile = Profile::GetInstance();
	profile.Begin("Move_Packet");
	*Buffer >> x >> y >> z >> yaw >> pitch >> roll >> vx >> vy >> vz;

	users[Index].UpdateCharacterMove(x, y, z
		, DequantizeInt16ToFloat(yaw), DequantizeInt16ToFloat(pitch), DequantizeInt16ToFloat(roll)
		, DequantizeInt16ToFloat(vx), DequantizeInt16ToFloat(vy), DequantizeInt16ToFloat(vz));
	profile.End("Move_Packet");
	printf("Position : %f %f %f \n", x, y, z);
	printf("Rotation : %f %f %f \n", DequantizeInt16ToFloat(yaw), DequantizeInt16ToFloat(pitch), DequantizeInt16ToFloat(roll));
	printf("Velocity : %f %f %f \n", DequantizeInt16ToFloat(vx), DequantizeInt16ToFloat(vy), DequantizeInt16ToFloat(vz));
}

void RPGServer::PacketProc_Move_ClientV1(int Index, CMessageBuffer* Buffer)
{
	float x, y, z;
	float yaw, pitch, roll;
	float vx, vy, vz;

	Profile& profile = Profile::GetInstance();
	profile.Begin("Move_Packet_V1");
	*Buffer >> x >> y >> z >> yaw >> pitch >> roll >> vx >> vy >> vz;

	users[Index].UpdateCharacterMove(x, y, z
		, yaw, pitch, roll
		, vx, vy, vz);
	profile.End("Move_Packet_V1");
	printf("Position : %f %f %f \n", x, y, z);
	printf("Rotation : %f %f %f \n", (yaw), (pitch), (roll));
	printf("Velocity : %f %f %f \n", (vx), (vy), (vz));
}