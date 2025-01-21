#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <process.h>
#include <atomic>
#include <mutex>
#include <optional>

#include "Interfaces/ILock.h"

#include "SpinLock.h"
#include "CLockStack.h"
#include "CSession.h"
#include "CSetting.h"
#include "CServer.h"

#pragma comment(lib, "ws2_32")


CServer::~CServer()
{
	delete sessionIndex;
}

bool CServer::Start(const int InSessionCount, const CSetting& InSetting)
{
	WSADATA wsa = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}

	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		return false;
	}

	SOCKADDR_IN serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(InSetting.GetPort());
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		return false;
	}

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		return false;
	}

	sessionIndex = new CLockStack<int>(new SpinLock());
	session.reserve(InSessionCount);
	for (int i = 0; i < InSessionCount; ++i)
	{
		sessionIndex->Push(i);
		auto tmpSession = std::make_unique<CSession>();
		session.emplace_back(std::move(tmpSession));
	}
	
	hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	auto threadHandle = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, (LPVOID)this, 0, 0);
	CloseHandle(threadHandle);
	threadHandle = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, (LPVOID)this, 0, 0);
	CloseHandle(threadHandle);

	InitMonitoring();
}

bool CServer::RecvPost(CSession* InSession)
{
	WSABUF wsa;
	DWORD flags = 0;
	int retRecvCount = 0;

	wsa.buf = InSession->GetBufferPtr();
	wsa.len = 256;

	WSAOVERLAPPED* recv = InSession->GetRecvOverlapped();
	retRecvCount = WSARecv(InSession->GetSocket(), &wsa, 1, NULL, &flags, recv, NULL);
	if (retRecvCount == SOCKET_ERROR)
	{
		auto error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			return false;
		}
	}

	return true;
}

unsigned int WINAPI CServer::AcceptThread(LPVOID lpParam)
{
	CServer* server = (CServer*)lpParam;
	SOCKADDR_IN clientAddr = { 0 };
	SOCKET clientSocket = { 0 };
	int addrLen = sizeof(clientAddr);
	std::optional<int> sessionIndexCheck;
	int sessionIndex = 0;
	while (1)
	{
		clientSocket = accept(server->listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			auto error = WSAGetLastError();
			if (error == WSAENOBUFS || error == WSAENETDOWN)
				break;
		}

		sessionIndexCheck = server->sessionIndex->Pop();
		if (!sessionIndexCheck.has_value())
			continue;
			
		sessionIndex = sessionIndexCheck.value();

		CSession* session = server->session[sessionIndex].get();

		session->Init(clientSocket);

		puts("Session Accept");

		CreateIoCompletionPort((HANDLE)session->GetSocket(), server->hIocp, (ULONG_PTR)session, 0);

		if (server->RecvPost(session))
		{
			++server->acceptTPS;
			++server->sessionCount;
		}
		else
		{
			// session release
			puts("[Accept Thread] session recv false return");
		}
	}
	return 0;
}

unsigned int WINAPI CServer::WorkerThread(LPVOID lpParam)
{
	CServer* server = (CServer*)lpParam;
	DWORD transferred = 0;
	CSession* session = nullptr;
	OVEREX* overlapped = nullptr;
	BOOL retGQCS = false;
	while (1)
	{
		retGQCS = GetQueuedCompletionStatus(server->hIocp, &transferred, (PULONG_PTR)&session, (LPOVERLAPPED*)&overlapped, INFINITE);
		if (retGQCS)
		{
			if (transferred == 0)
			{
				if (session == NULL && overlapped == NULL)
				{
					// 워커스레드 작업 종료
				}
				else if (session != NULL)
				{
					// 클라이언트 연결 종료
				}
			}
			else
			{
				if (overlapped->type == OVEREX::RECV)
				{

				}
				else if (overlapped->type == OVEREX::SEND)
				{

				}
			}
		}
		else
		{
			//완료 큐에서 패킷을 꺼내지 못하고 반환 + IOCP 핸들 닫힘
			if (overlapped == NULL)
			{
				puts("[Worker Thread][GQCS] IOCP 핸들 닫힘");
				break;
			}
			else
			{
				// 클라이언트 비정상 종료 + 서버 연결 종료 요청
				if (session != NULL)
				{
					// 세선 종료 절차 밟기
				}
				puts("[Worker Thread][GQCS] 서버 종료 혹은 비정상 연결 종료");
			}
		}
	}
	

	return 0;
}

void CServer::ResetMonitoring()
{
	recvTPS = 0;
	sendTPS = 0;
	acceptTPS = 0;
}

void CServer::InitMonitoring()
{
	sessionCount = 0;
	recvTPS = 0;
	sendTPS = 0;
	acceptTPS = 0;
}

