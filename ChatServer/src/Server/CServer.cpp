#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <process.h>
#include "CSession.h"
#include "CSetting.h"
#include "CServer.h"

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

	session.resize(InSessionCount);

	auto threadHandle = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, (LPVOID)this, 0, 0);
	CloseHandle(threadHandle);
}

unsigned int WINAPI CServer::AcceptThread(LPVOID lpParam)
{
	CServer* server = (CServer*)lpParam;
	SOCKADDR_IN clientAddr = { 0 };
	SOCKET clientSocket = { 0 };
	int addrLen = sizeof(clientAddr);
	while (1)
	{
		clientSocket = accept(server->listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			auto error = WSAGetLastError();
			if (error == WSAENOBUFS || error == WSAENETDOWN)
				break;
		}
	}
	return 0;
}

