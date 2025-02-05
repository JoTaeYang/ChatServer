#include "ChatServer/Server/ChatServer.h"

#include <iostream>

#include "ChatServer/User/User.h"

bool ChatServer::Start(const int InSessionCount, const CSetting& InSetting)
{
	CServer::Start(InSessionCount, InSetting);

	users.reserve(InSessionCount);
	for (int i = 0; i < InSessionCount; ++i)
	{
		auto tmpUser = std::make_unique<User>();
		users.emplace_back(std::move(tmpUser));
	}


	return true;
}

/*
������ Ȱ��ȭ ��Ų��.
*/
void ChatServer::OnJoin()
{
	
}

void ChatServer::OnRecv()
{

}
