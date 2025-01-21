#pragma once


class CServer
{
public:
	CServer() = delete;
	virtual ~CServer();

	bool Start(const int InSessionCount, const class CSetting &InSetting);


private:
	SOCKET listenSocket;
	std::vector<class CSession> session;

// Thread Function Section
private:
	static unsigned int WINAPI AcceptThread(LPVOID lpParam);
};

