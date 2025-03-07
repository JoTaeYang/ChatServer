#include "Library/Session/CSession.h"

#include <WinSock2.h>
#include <Windows.h>
#include <strsafe.h>
#include <time.h>
#include "Library/RingBuffer/RingBuffer.h"
#include "Library/Lock/SpinLock/SpinLock.h"

CSession::CSession() : _socket(INVALID_SOCKET), index(0), buffer(), completeRecvBuffer(new SpinLock()), sendBuffer(new SpinLock)
{
	this->recvOverlapped.type = OVEREX::RECV;
	this->sendOverlapped.type = OVEREX::SEND;
}


CSession::CSession(CSession&& session)  noexcept : completeRecvBuffer(new SpinLock()), sendBuffer(new SpinLock)
{	
	this->_socket = session._socket;
	this->index = session.index;	
}

CSession::CSession(const CSession& session) : _socket(session._socket), index(session.index), buffer(), completeRecvBuffer(new SpinLock()), sendBuffer(new SpinLock)
{
}

void CSession::Init(SOCKET& socket, unsigned short index)
{
	this->_socket = socket;
	this->buffer.Clear();
	this->index = index;
	this->sendFlag = 0;
	this->status = SessionStatus::GAME;
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

void CSession::RecvToComplete(CMessageBuffer* InBuffer)
{
	completeRecvBuffer.Enqueue(InBuffer);	
}

bool CSession::TrySend()
{
	return InterlockedExchange(&sendFlag, 1) == 0;
}

int CSession::UpdateSendFlag(int value)
{
	return InterlockedExchange(&sendFlag, value);
}

bool CSession::PopCompleteBuffer(CMessageBuffer*& OutBuffer)
{		
	return completeRecvBuffer.Dequeue(OutBuffer);
}

void CSession::SendQEnqueue(CMessageBuffer* InBuffer)
{
	sendBuffer.Enqueue(InBuffer);
}

int CSession::SendQPeek(CMessageBuffer*& OutBuffer, int _pos)
{
	return sendBuffer.Peek(OutBuffer, _pos);
}

bool CSession::SendPacket(WSABUF* buf, int bufferCount)
{
	ZeroMemory(&sendOverlapped, sizeof(WSAOVERLAPPED));
	return WSASend(_socket, buf, bufferCount, NULL, 0, (WSAOVERLAPPED*)&sendOverlapped, NULL);
}