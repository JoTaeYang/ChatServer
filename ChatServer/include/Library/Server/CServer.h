#pragma once

template<typename T>
class CLockStack;

class CServer
{
public:
	CServer() = default;
	virtual ~CServer();

	bool Start(const int InSessionCount, const class CSetting &InSetting);


private:
	std::vector<std::unique_ptr<class CSession>> session;
	CLockStack<int>* sessionIndex;

	SOCKET listenSocket;
	HANDLE hIocp;

// Networking Function Section
private:
	bool RecvPost(CSession* InSession);
	void CompleteRecv(CSession* InSession, DWORD transferred);

// Thread Function Section
private:
	static unsigned int WINAPI AcceptThread(LPVOID lpParam);
	static unsigned int WINAPI WorkerThread(LPVOID lpParam);

// Monitoring
public:
	void ResetMonitoring();

private:
	std::atomic<int> sessionCount;
	std::atomic<int> recvTPS;
	std::atomic<int> sendTPS;
	std::atomic<int> acceptTPS;

	void InitMonitoring();
};

