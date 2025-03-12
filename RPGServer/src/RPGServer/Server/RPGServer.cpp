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
	
	return true;
}

/*
세션을 활성화 시킨다.
*/
void RPGServer::OnJoin(int Index)
{
	users[Index].Init();

	// 유저 캐릭터 생성
	CMessageBuffer* pBuffer = CMessageBuffer::Alloc();

	CreatePlayer(Index, pBuffer);

	SendPacket(Index, pBuffer);


	// 나 -> 타인
	CMessageBuffer* createThisOtherUsers = CMessageBuffer::Alloc();

	// 다른 유저들한테 Join한 캐릭터 생성하는 메시지 생성
	MakePacketCreateOtherPlayer(Index, createThisOtherUsers);

	// TODO 전송하기

	// 타인 -> 나
	CMessageBuffer* createOtherUsers = NULL;
	for (int i = 0; i < SessionCount; ++i)
	{
		if (users[i].IsGame())
		{
			// 내가 아닌 다른 캐릭터를 나에게 생성하라고 전송하기 
			if (Index != i)
			{
				createOtherUsers = CMessageBuffer::Alloc();

				MakePacketCreateOtherPlayer(Index, createOtherUsers);

				SendPacket(Index, createOtherUsers);
				createOtherUsers->DecRef();

				// 다른 캐릭터에게 나를 생성하라고 전송하기
				SendPacket(i, createThisOtherUsers);
			}			
		}
	}

	pBuffer->DecRef();
	createThisOtherUsers->DecRef();
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

void RPGServer::OnLeave(int Index)
{

}

void RPGServer::CreatePlayer(int Index, CMessageBuffer* buffer)
{
	Header header;

	float posX, posY = 0.0f, posZ = 0.0f;
	float yaw = 0.1f, pitch = 0.1f, roll = 0.1f;

	std::random_device t;
	std::mt19937 gen(t());
	std::uniform_int_distribution<int> dis(500, 1000);
	std::uniform_int_distribution<int> dis2(500, 1000);
		
	header.byCode = 0x89;
	header.wType = EPacketType::SC_CREATE_MY;
	header.bySize = 24;

	posX = dis(gen);
	posY = dis2(gen);
	std::cout << posX << "  " << posZ << "\n";
	buffer->PutData((char*)&header, sizeof(Header));

	this->users[Index].InitPlayer(posX, posY, posZ, yaw, pitch, roll, 0, 0, 0);

	*buffer << posX << posY << posZ << yaw << pitch << roll;
}

void RPGServer::MakePacketCreateOtherPlayer(int Index, CMessageBuffer* buffer)
{
	Header header;
	float posX, posY, posZ, yaw, pitch, roll;

	header.byCode = 0x89;
	header.wType = EPacketType::SC_CREATE_OTHER;
	header.bySize = 24;

	
	users[Index].GetCreatePosRot(posX, posY, posZ, yaw, pitch, roll);

	buffer->PutData((char*)&header, sizeof(Header));
	*buffer << posX << posY << posZ << yaw << pitch << roll;	
}

void RPGServer::SendAround(int Index)
{

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