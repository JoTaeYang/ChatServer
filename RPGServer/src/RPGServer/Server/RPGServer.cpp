#include "RPGServer/Server/RPGServer.h"

#include <iostream>

#include "Library/Packet/Header.h"
#include "Library/MessageBuffer/MessageBuffer.h"

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

void RPGServer::PacketProc_Move_Client(int Index, CMessageBuffer* Buffer)
{
	float x, y, z;
	short yaw, pitch, roll;
	short vx, vy, vz;

	*Buffer >> x >> y >> z >> yaw >> pitch >> roll >> vx >> vy >> vz;

	printf("Position : %f %f %f \n", x, y, z);
	printf("Rotation : %f %f %f \n", DequantizeInt16ToFloat(yaw), DequantizeInt16ToFloat(pitch), DequantizeInt16ToFloat(roll));
	printf("Velocity : %f %f %f \n", DequantizeInt16ToFloat(vx), DequantizeInt16ToFloat(vy), DequantizeInt16ToFloat(vz));
}
