#pragma once



template<typename T>
class CLockStack;
class CSession;

#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#pragma comment(lib, MYSQL_LIB_DEBUG)
#else
#pragma comment(lib, MYSQL_LIB_RELEASE)
#endif

#include <WinSock2.h>
#include <atomic>
#include <string>
#include <memory>


class CServer
{
public:
	CServer() = default;
	virtual ~CServer();

	bool Start(const int InSessionCount, const class CSetting &InSetting, const class CRedisDB* Redis);
	bool Stop();

	const int GetSessionCount() { return SessionCount; }
private:
	CSession* session;
	CLockStack<int>* sessionIndex;

	SOCKET listenSocket;
	HANDLE hIocp;

protected:
	int SessionCount;
	const class CRedisDB* redisDB;

// Virtual Section
protected:
	virtual void OnJoin(int Index) = 0;
	virtual void OnRecv(int Index, class CMessageBuffer* Buffer) = 0;
	virtual void OnLeave(int Index) = 0;
	virtual bool OnSessionKey(int Index) = 0;

// Networking Function Section
private:
	bool RecvPost(CSession* InSession);
	bool SendPost(CSession* InSession);

	void CompleteRecv(CSession* InSession, DWORD transferred);
	void CompleteSend(CSession* InSession, DWORD transferred);

	void GameProcess();
	void ReleaseCheckProcess();
	void EnterGameCheckProcess();
protected:
	bool SendPacket(int Index, class CMessageBuffer* Buffer);

// Thread Function Section
private:
	static unsigned int WINAPI AcceptThread(LPVOID lpParam);
	static unsigned int WINAPI WorkerThread(LPVOID lpParam);
	static unsigned int WINAPI LogicThread(LPVOID lpParam);
	static unsigned int WINAPI SendThread(LPVOID lpParam);

// Monitoring
public:
	void ResetMonitoring();

	std::atomic<int> exitCheck;
private:
	std::atomic<int> sessionCount;
	std::atomic<int> recvTPS;
	std::atomic<int> sendTPS;
	std::atomic<int> acceptTPS;

	std::atomic<int> recvCall;

	void InitMonitoring();
};

