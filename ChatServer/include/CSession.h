#pragma once

class CSession
{
public:
	CSession();

	void Init();					// 각 필드를 NULL로 초기화 하는 함수

private:
	SOCKET _socket;					// 소켓,8byte	
	char buffer[256];				// 세션의 버퍼, 256byte
	unsigned short index;			// 서버 세션 관리 객체에서 세션의 인덱스, 2byte



};

