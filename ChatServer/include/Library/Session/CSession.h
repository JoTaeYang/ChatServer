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

	void Init(SOCKET& socket);					// �� �ʵ带 NULL�� �ʱ�ȭ �ϴ� �Լ�

	const SOCKET GetSocket() const;
	CRingBuffer* GetRecvRingBuffer();

	OVEREX* GetRecvOverlapped();

private:
	CRingBuffer buffer;	
	SOCKET _socket;					// ����,8byte	
	unsigned short index;			// ���� ���� ���� ��ü���� ������ �ε���, 2byte

	OVEREX recvOverlapped;
	OVEREX sendOverlapped;
};

