#pragma once


class CServer
{
public:
	CServer() = delete;
	virtual ~CServer();

	bool Start(const int InSessionCount);


private:
	SOCKET listenSocket;
	std::vector<class CSession> session;
};

