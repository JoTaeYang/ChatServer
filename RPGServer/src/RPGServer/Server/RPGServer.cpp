#include "RPGServer/Server/RPGServer.h"

#include <iostream>

#include "RPGServer/User/User.h"

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
������ Ȱ��ȭ ��Ų��.
*/
void RPGServer::OnJoin(int Index)
{
	users[Index].Init();
}

void RPGServer::OnRecv(int Index)
{

}
