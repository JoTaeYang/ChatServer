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

	void RecvToComplete(class CMessageBuffer* InBuffer);

	bool IsOnGame() {
		return (status == SessionStatus::GAME);
	}

	bool IsSending() {
		return sendFlag == 1;
	}

	bool PopCompleteBuffer(CMessageBuffer*& OutBuffer);
	void SendQEnqueue(class CMessageBuffer* InBuffer);

private:
	CRingBuffer buffer;	
	CLockQueue<class CMessageBuffer*> completeRecvBuffer;
	CLockQueue<class CMessageBuffer*> sendBuffer;
	SessionStatus status;
	OVEREX recvOverlapped;
	OVEREX sendOverlapped;

	SOCKET _socket;					// ����,8byte		
	unsigned short index;			// ���� ���� ���� ��ü���� ������ �ε���, 2byte

	DWORD sendFlag;
};

