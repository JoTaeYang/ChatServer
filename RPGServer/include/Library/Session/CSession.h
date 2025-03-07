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

	int SendCount() {
		return this->sendBuffer.GetCount();
	}

	bool TrySend();
	int UpdateSendFlag(int value);

	bool PopCompleteBuffer(CMessageBuffer*& OutBuffer);

	void SendQEnqueue(class CMessageBuffer* InBuffer);
	int SendQPeek(CMessageBuffer*& OutBuffer, int _pos);
	bool SendPacket(WSABUF* buf, int bufferCount);

	OVEREX sendOverlapped;
private:
	CRingBuffer buffer;	
	CLockQueue<class CMessageBuffer*> completeRecvBuffer;
	CLockQueue<class CMessageBuffer*> sendBuffer;
	SessionStatus status;
	OVEREX recvOverlapped;
	

	SOCKET _socket;					// ����,8byte		
	unsigned short index;			// ���� ���� ���� ��ü���� ������ �ε���, 2byte

	DWORD sendFlag;
};

