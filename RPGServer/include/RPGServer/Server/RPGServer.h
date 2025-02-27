#pragma once

#include "Library/Server/CServer.h"

class RPGServer : public CServer
{
public:
	bool Start(const int InSessionCount, const class CSetting& InSetting);

protected:
	virtual void OnJoin(int Index) override;
	virtual void OnRecv(int Index, class CMessageBuffer* Buffer) override;

private:
	class User* users;	
};