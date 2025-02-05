#pragma once



template<typename T>
class CLockStack;
class CSession;

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <atomic>
#include <vector>
#include <memory>


class CServer
{
public:
	CServer() = default;
	virtual ~CServer();

	bool Start(const int InSessionCount, const class CSetting &InSetting);


private:
	std::vector<std::unique_ptr<CSession>> session;
	CLockStack<int>* sessionIndex;

	SOCKET listenSocket;
	HANDLE hIocp;

// Virtual Section
protected:
	virtual void OnJoin() = 0;
	virtual void OnRecv() = 0;

// Networking Function Section
private:
	bool RecvPost(CSession* InSession);
	void CompleteRecv(CSession* InSession, DWORD transferred);

// Thread Function Section
private:
	static unsigned int WINAPI AcceptThread(LPVOID lpParam);
	static unsigned int WINAPI WorkerThread(LPVOID lpParam);
	static unsigned int WINAPI LogicThread(LPVOID lpParam);

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

