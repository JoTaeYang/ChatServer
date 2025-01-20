#include <vector>
#include <WinSock2.h>
#include "CSession.h"
#include "CServer.h"

#pragma comment(lib, "ws2_32")


CServer::~CServer()
{

}

bool CServer::Start(const int InSessionCount)
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
	serverAddr.sin_port = htons(30000);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//bind(listenSocket)

	//session.resize(InSessionCount);
}
