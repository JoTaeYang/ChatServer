#pragma once

class CSession
{
public:
	CSession(SOCKET InSocket, unsigned short InIndex) : _socket(InSocket), index(InIndex) { }

	void Init();					// �� �ʵ带 NULL�� �ʱ�ȭ �ϴ� �Լ�

private:
	SOCKET _socket;					// ����,8byte	
	char buffer[256];				// ������ ����, 256byte
	unsigned short index;			// ���� ���� ���� ��ü���� ������ �ε���, 2byte
};

