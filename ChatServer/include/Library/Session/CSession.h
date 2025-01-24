#pragma once
#include "Library/RingBuffer/RingBuffer.h"

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
public:
	CSession();
	CSession(CSession&& session) noexcept;
	CSession(const CSession& session);

	void Init(SOCKET& socket);					// 각 필드를 NULL로 초기화 하는 함수

	const SOCKET GetSocket() const;
	CRingBuffer* GetRecvRingBuffer();

	OVEREX* GetRecvOverlapped();

private:
	CRingBuffer buffer;	
	SOCKET _socket;					// 소켓,8byte	
	unsigned short index;			// 서버 세션 관리 객체에서 세션의 인덱스, 2byte

	OVEREX recvOverlapped;
	OVEREX sendOverlapped;
};

