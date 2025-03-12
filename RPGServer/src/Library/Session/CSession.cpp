#include "Library/Session/CSession.h"

#include <WinSock2.h>
#include <Windows.h>
#include <strsafe.h>
#include <iostream>
#include <time.h>
#include "Library/RingBuffer/RingBuffer.h"
#include "Library/MessageBuffer/MessageBuffer.h"
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
	this->IOCount = 0;
	this->status = SessionStatus::GAME;

	ZeroMemory(&this->recvOverlapped, sizeof(WSAOVERLAPPED));
	ZeroMemory(&this->sendOverlapped, sizeof(WSAOVERLAPPED));
}

void CSession::Release()
{
	CMessageBuffer* buffer = NULL;

	while (completeRecvBuffer.Dequeue(buffer))
	{
		if (!buffer) break;

		buffer->DecRef();
	}

	while (sendBuffer.Dequeue(buffer))
	{
		if (!buffer) break;

		buffer->DecRef();
	}

	closesocket(_socket);

	_socket = INVALID_SOCKET;

	status = SessionStatus::NONE;
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

bool CSession::PopCompleteSendBuffer(CMessageBuffer*& OutBuffer)
{
	return sendBuffer.Dequeue(OutBuffer);
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

void CSession::IOCountDecre()
{
	if (InterlockedDecrement(&IOCount) == 0)
	{
		if (IOCount < 0)
			std::cout << "IOCount Minus Check\n";
		this->status = SessionStatus::LOGOUT;
	}
}
