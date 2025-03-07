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
	

	SOCKET _socket;					// 소켓,8byte		
	unsigned short index;			// 서버 세션 관리 객체에서 세션의 인덱스, 2byte

	DWORD sendFlag;
};

