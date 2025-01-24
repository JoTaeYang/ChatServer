#include <WinSock2.h>
#include <Windows.h>
#include <strsafe.h>
#include <time.h>
#include "Library/RingBuffer/RingBuffer.h"
#include "Library/Session/CSession.h"

CSession::CSession() : _socket(0), index(0), buffer()
{
	this->recvOverlapped.type = OVEREX::RECV;
	this->sendOverlapped.type = OVEREX::SEND;
}


CSession::CSession(CSession&& session) noexcept
{	
	this->_socket = session._socket;
	this->index = session.index;	
}

CSession::CSession(const CSession& session) : _socket(session._socket), index(session.index), buffer()
{
}

void CSession::Init(SOCKET& socket)
{
	this->_socket = socket;
	this->buffer.Clear();	
	this->index = -1;
	ZeroMemory(&this->recvOverlapped, sizeof(WSAOVERLAPPED));
	ZeroMemory(&this->sendOverlapped, sizeof(WSAOVERLAPPED));
}

const SOCKET CSession::GetSocket() const
{
	return this->_socket;
}

CRingBuffer* CSession::GetRecvRingBuffer()
{
	return &(this->buffer);
}

OVEREX* CSession::GetRecvOverlapped()
{
	return &(this->recvOverlapped);
}
