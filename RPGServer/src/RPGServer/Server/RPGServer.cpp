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
		std::cout << "Recv Move Packet!!";
		break;
	}
}
