#include <WinSock2.h>
#include "CSession.h"

CSession::CSession() : _socket(0), index(0), buffer{0}
{
	this->recvOverlapped.type = OVEREX::RECV;
	this->sendOverlapped.type = OVEREX::SEND;
}

CSession::CSession(CSession&& session) noexcept
{	
	this->_socket = session._socket;
	this->index = session.index;	
	ZeroMemory(this->buffer, sizeof(this->buffer));
}

CSession::CSession(const CSession& session) : _socket(session._socket), index(session.index), buffer{ 0 }
{
}

void CSession::Init(SOCKET& socket)
{
	this->_socket = socket;
	ZeroMemory(this->buffer, sizeof(this->buffer));
	this->index = -1;
	ZeroMemory(&this->recvOverlapped, sizeof(WSAOVERLAPPED));
	ZeroMemory(&this->sendOverlapped, sizeof(WSAOVERLAPPED));
}

const SOCKET CSession::GetSocket() const
{
	return this->_socket;
}

char* CSession::GetBufferPtr() 
{
	return this->buffer;
}

OVEREX* CSession::GetRecvOverlapped()
{
	return &(this->recvOverlapped);
}
