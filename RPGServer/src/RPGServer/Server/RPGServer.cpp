#include "RPGServer/Server/RPGServer.h"

#include <iostream>
#include <random>
#include <algorithm>
#include <iomanip>
#include <sstream>


#include "Library/Packet/Header.h"
#include "Library/MessageBuffer/MessageBuffer.h"
#include "Library/Profile/Profile.h"
#include "Library/DB/CRedisDB.h"
#include "Library/DB/IRepository.h"

#include "RPGServer/User/User.h"
#include "RPGServer/Protocol/PacketType.h"
#include "RPGServer/Datas/SessionInfo.h"


bool RPGServer::Start(const int InSessionCount, const CSetting& InSetting, const CRedisDB* Redis)
{
	CServer::Start(InSessionCount, InSetting, Redis);

	users = new User[InSessionCount];
	
	return true;
}

/*
세션을 활성화 시킨다.
*/
void RPGServer::OnJoin(int Index)
{
	users[Index].Init();
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
	case EPacketType::CS_AUTH:
		PacketProc_Auth(Index, Buffer);
		break;
	case EPacketType::CS_MOVE_MY:
		PacketProc_Move_Client(Index, Buffer);	
		break;
	}
}

void RPGServer::OnLeave(int Index)
{

}

bool RPGServer::OnSessionKey(int Index)
{
	if (users[Index].sessionKey != "")
	{
		SessionInfoRepository repo;

		SessionInfo infos;
		repo.GetUser((IRepository *)redisDB, users[Index].sessionKey, infos);

		if (infos.Uid != "")
		{
			std::cout << "Redis UID Check : " << infos.Uid << "\n";
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
			return true;
		}		
	}		
	return false;
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
	header.bySize = 40;

	
	users[Index].GetCreatePosRot(posX, posY, posZ, yaw, pitch, roll);

	buffer->PutData((char*)&header, sizeof(Header));
	buffer->PutData(users[Index].sessionHex, sizeof(users[Index].sessionHex));
	*buffer << posX << posY << posZ << yaw << pitch << roll;	
}

void RPGServer::MakePacketCreateMovePlayer(int Index, CMessageBuffer* buffer)
{
	Header header;
	float posX, posY, posZ, yaw, pitch, roll, vX, vY, vZ;

	header.byCode = 0x89;
	header.wType = EPacketType::SC_MOVE_CHAR;
	header.bySize = 40;

	users[Index].GetMoveInfos(posX, posY, posZ, yaw, pitch, roll, vX, vY, vZ);

	buffer->PutData((char*)&header, sizeof(Header));
	buffer->PutData(users[Index].sessionHex, sizeof(users[Index].sessionHex));
	*buffer << posX << posY << posZ 
		<< QuantizeFloatToInt16(yaw) << QuantizeFloatToInt16(pitch) << QuantizeFloatToInt16(roll)
		<< QuantizeFloatToInt16(vX, true) << QuantizeFloatToInt16(vY, true) << QuantizeFloatToInt16(vZ, true);
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
		, DequantizeInt16ToFloat(vx, true), DequantizeInt16ToFloat(vy, true), DequantizeInt16ToFloat(vz, true));

	profile.End("Move_Packet");

	CMessageBuffer* otherMove = CMessageBuffer::Alloc();

	MakePacketCreateMovePlayer(Index, otherMove);
	for (int i = 0; i < SessionCount; ++i)
	{
		if (users[i].IsGame())
		{
			// 다른 캐릭터에게 나를 움직이게 하기
			if (Index != i)
			{
				SendPacket(i, otherMove);				
			}
		}
	}
	otherMove->DecRef();
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

void RPGServer::PacketProc_Auth(int Index, CMessageBuffer* Buffer)
{
	std::ostringstream oss;

	char a[16];

	Buffer->GetData(a, 16);
	for (uint8_t byte : a)
	{
		oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte); 
	}

	users[Index].sessionKey = std::move(oss.str());	
	memcpy_s(users[Index].sessionHex, 16, a, 16);
}

short RPGServer::QuantizeFloatToInt16(float value, bool isVelocity)
{
	

	if (isVelocity)
	{
		value = std::clamp(value, VX_MIN, VX_MAX);
		return static_cast<short>(
			(value - VX_MIN) * (SHRT_MAX - SHRT_MIN) / (VX_MAX - VX_MIN) + SHRT_MIN
			);
	}
	else
	{
		value = std::clamp(value, X_MIN, X_MAX);
		return static_cast<short>(
			(value - X_MIN) * (SHRT_MAX - SHRT_MIN) / (X_MAX - X_MIN) + SHRT_MIN
			);
	}
}

