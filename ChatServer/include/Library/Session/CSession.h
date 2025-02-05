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

	void Init(SOCKET& socket, unsigned short index);					// �� �ʵ带 NULL�� �ʱ�ȭ �ϴ� �Լ�

	const SOCKET GetSocket() const;
	CRingBuffer* GetRecvRingBuffer();

	OVEREX* GetRecvOverlapped();

private:
	CRingBuffer buffer;	
	CLockQueue<char*> completeRecvBuffer;
	
	OVEREX recvOverlapped;
	OVEREX sendOverlapped;

	SOCKET _socket;					// ����,8byte	
	unsigned short index;			// ���� ���� ���� ��ü���� ������ �ε���, 2byte

	SessionStatus status;
};

