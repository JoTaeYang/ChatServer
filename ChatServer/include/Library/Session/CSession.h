#pragma once
#include <WinSock2.h>

#include "Library/RingBuffer/RingBuffer.h"
#include "Library/LockQueue/CLockQueue.h"


struct OVEREX : WSAOVERLAPPED
{
	enum OverlapType {
		RECV,
		SEND
	};

	OverlapType type;
};

class CSession
{
private:
	enum class SessionStatus {
		NONE,
		LOGIN,

		GAME,
	};
public:
	CSession();
	CSession(CSession&& session) noexcept;
	CSession(const CSession& session);

	void Init(SOCKET& socket, unsigned short index);					// 각 필드를 NULL로 초기화 하는 함수

	const SOCKET GetSocket() const;
	CRingBuffer* GetRecvRingBuffer();

	OVEREX* GetRecvOverlapped();

private:
	CRingBuffer buffer;	
	CLockQueue<char*> completeRecvBuffer;
	
	OVEREX recvOverlapped;
	OVEREX sendOverlapped;

	SOCKET _socket;					// 소켓,8byte	
	unsigned short index;			// 서버 세션 관리 객체에서 세션의 인덱스, 2byte

	SessionStatus status;
};

