#include "Library/Server/CServer.h"

#include <iostream>
#include <process.h>
#include <mutex>
#include <optional>
#include <WinSock2.h>

#include "Library/Interface/ILock.h"

#include "Library/Packet/Header.h"
#include "Library/Packet/PacketType.h"

#include "Library/Lock/SpinLock/SpinLock.h"
#include "Library/LockStack/CLockStack.h"
#include "Library/Session/CSession.h"
#include "Library/Setting/CSetting.h"
#include "Library/MessageBuffer/MessageBuffer.h"

#pragma comment(lib, "ws2_32")


CServer::~CServer()
{
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
	session = new CSession[InSessionCount];	
	for (int i = 0; i < InSessionCount; ++i)
	{
		sessionIndex->Push(i);	
	}
	
	hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	SessionCount = InSessionCount;

	auto threadHandle = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, (LPVOID)this, 0, 0);
	CloseHandle(threadHandle);
	threadHandle = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, (LPVOID)this, 0, 0);
	CloseHandle(threadHandle);
	threadHandle = (HANDLE)_beginthreadex(NULL, 0, LogicThread, (LPVOID)this, 0, 0);
	CloseHandle(threadHandle);
	threadHandle = (HANDLE)_beginthreadex(NULL, 0, SendThread, (LPVOID)this, 0, 0);
	CloseHandle(threadHandle);

	InitMonitoring();
	return true;
}

bool CServer::Stop()
{
	exitCheck += 1;
	CloseHandle(hIocp);

	for (int i = 0; i < SessionCount; i++)
	{
		if (const SOCKET socket = session[i].GetSocket() != INVALID_SOCKET)
		{
			closesocket(socket);
		}		
	}
	
	delete sessionIndex;
	delete[] session;

	closesocket(listenSocket);

	WSACleanup();
	return true;
}

bool CServer::RecvPost(CSession* InSession)
{
	WSABUF wsa;
	CRingBuffer* buffer = InSession->GetRecvRingBuffer();
	DWORD flags = 0;
	int retRecvCount = 0;

	wsa.buf = buffer->GetRearBufferPtr();
	wsa.len = buffer->DirectEnqueueSize();

	WSAOVERLAPPED* recv = InSession->GetRecvOverlapped();
	retRecvCount = WSARecv(InSession->GetSocket(), &wsa, 1, NULL, &flags, recv, NULL);
	if (retRecvCount == SOCKET_ERROR)
	{
		auto error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			puts("[Recv Post] Recv IO Error");
			return false;
		}
	}

	return true;
}

bool CServer::SendPost(CSession* InSession)
{
	WSABUF wsaBuf[100] = { 0, };
	int bufferCount = 0;
	int sendCount = InSession->SendCount();
	if (sendCount < 0)
	{
		return true;
	}

	if (InSession->TrySend())
	{
		CMessageBuffer* buffer = NULL;

		if (InSession->SendCount() <= 0)
		{
			InSession->UpdateSendFlag(0);
			return true;
		}

		while (InSession->SendCount() > bufferCount)
		{
			if (bufferCount >= 100)
				break;

			buffer = NULL;

			if (InSession->SendQPeek(buffer, bufferCount) == -1)
				break;

			if (buffer == NULL)
				break;

			wsaBuf[bufferCount].buf = (char*)buffer->GetBufferPtr();
			wsaBuf[bufferCount].len = buffer->GetBufferSize();

			++bufferCount;
		}

		if (bufferCount == 0)
		{
			InSession->UpdateSendFlag(0);
			return true;
		}


		ZeroMemory(&InSession->sendOverlapped, sizeof(WSAOVERLAPPED));
		DWORD flag = 0;
		if (WSASend(InSession->GetSocket(), wsaBuf, bufferCount, NULL, flag, (WSAOVERLAPPED*)&InSession->sendOverlapped, NULL) == SOCKET_ERROR)
		{
			auto err = WSAGetLastError();
			std::cout << "Session Error : " << err << "\n";
			if (err != ERROR_IO_PENDING)
			{
				InSession->UpdateSendFlag(0);
				//연결  끊기 절차 밟기
				return false;
			}
		}
	}
	return false;
}

void CServer::CompleteRecv(CSession* InSession, DWORD transferred)
{
	CRingBuffer* buffer = InSession->GetRecvRingBuffer();
	int bufferIdx = 0;
	Header header;	
	size_t headerSize = sizeof(Header);

	buffer->MoveRear(transferred);
	while (transferred > 0)
	{
		CMessageBuffer* msgBuffer = CMessageBuffer::Alloc();

		buffer->Peek(reinterpret_cast<char*>(&header), headerSize);
		if (header.byCode != 0x89)
		{
			printf("header Code : %x\n", header.byCode);
			msgBuffer->DecRef();
			break;
		}

		if (header.bySize + headerSize > transferred)
		{
			msgBuffer->DecRef();
			break;
		}
		
		int dequeSize = headerSize + header.bySize;
		buffer->Dequeue((char *)msgBuffer->GetBufferPtr(), dequeSize);
		msgBuffer->MoveWritePos(dequeSize);
		msgBuffer->AddRef();
		InSession->RecvToComplete(msgBuffer);
		msgBuffer->DecRef();
		transferred -= dequeSize;
	}

	if (!RecvPost(InSession))
	{
		puts("[Complete Recv] Recv Post Fin");
	}
}

bool CServer::SendPacket(int Index, CMessageBuffer* Buffer)
{
	CSession* tmpSession = &session[Index];

	Buffer->AddRef();

	tmpSession->SendQEnqueue(Buffer);

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

		// Nagle Disable
		BOOL nodelay = TRUE;
		if (setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&nodelay, sizeof(nodelay)) == SOCKET_ERROR) {			
			closesocket(clientSocket);
			continue;
		}

		sessionIndexCheck = server->sessionIndex->Pop();
		if (!sessionIndexCheck.has_value())
			continue;
			
		sessionIndex = sessionIndexCheck.value();

		CSession* session = &server->session[sessionIndex];

		session->Init(clientSocket, sessionIndex);

		server->OnJoin(sessionIndex);

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
					puts("[Worker Thread] Worker Thread Fin");
				}
				else if (session != NULL)
				{
					// 클라이언트 연결 종료
					puts("[Worker Thread] Client Connect Fin");
				}
			}
			else
			{
				if (overlapped->type == OVEREX::RECV)
				{
					server->CompleteRecv(session, transferred);
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

unsigned int WINAPI CServer::LogicThread(LPVOID lpParam)
{
	CServer* server = (CServer*)lpParam;
	const int SessionCount = server->GetSessionCount();
	CMessageBuffer* buffer = nullptr;
	int loop = 0;
	while (server->exitCheck == 0)
	{
		for (int i = 0; i < SessionCount; ++i)
		{
			if (server->exitCheck == 0 && server->session[i].IsOnGame())
			{
				loop = 0;
				while (loop < 2000)
				{
					buffer = NULL;
					if (!server->session[i].PopCompleteBuffer(buffer))
						break;
					if (buffer == NULL)
						break;
					server->OnRecv(i, buffer);
					buffer->DecRef();
				}
			}
			else if (server->exitCheck == 1)
				break;
		}
		Sleep(5);
	}
	return 0;
}

unsigned int __stdcall CServer::SendThread(LPVOID lpParam)
{
	CServer* server = (CServer*)lpParam;
	const int SessionCount = server->GetSessionCount();
	while (server->exitCheck == 0)
	{
		for (int i = 0; i < SessionCount; ++i)
		{
			if (server->exitCheck == 0 && server->session[i].IsOnGame())
			{
				if (!server->session[i].IsSending())
				{
					if (server->SendPost(&server->session[i]))
					{
						// TODO :: 뭘 해줘야 할지 ,, 좀 고민
					}
				}
			}
			else if (server->exitCheck == 1)
				break;
		}
		Sleep(5);
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

