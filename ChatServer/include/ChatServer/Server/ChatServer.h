#pragma once

#include "Library/Server/CServer.h"

class ChatServer : public CServer
{
public:
	bool Start(const int InSessionCount, const class CSetting& InSetting);

protected:
	virtual void OnJoin() override;
	virtual void OnRecv() override;

private:
	std::vector<std::unique_ptr<class User>> users;
};