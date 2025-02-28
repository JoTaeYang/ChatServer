#pragma once

#include <memory>
#include <vector>
#include <Windows.h>
#include "Library/MemoryPool/CMemoryPool.h"

class CMessageBuffer
{
public:
	CMessageBuffer();
	CMessageBuffer(int InBufferSize);
	~CMessageBuffer();

	void Clear(void);

public:
	int PutData(char* chpSrc, int iSrcSize);
	unsigned char* GetBufferPtr(void);
	int MoveWritePos(int iSize);
public:
	//넣기
	CMessageBuffer& operator <<(char chValue);
	CMessageBuffer& operator <<(unsigned char uchValue);

	CMessageBuffer& operator <<(short shValue);
	CMessageBuffer& operator <<(unsigned short ushValue);

	CMessageBuffer& operator <<(int iValue);
	CMessageBuffer& operator <<(unsigned int uiValue);

	CMessageBuffer& operator <<(float fValue);

	CMessageBuffer& operator <<(long ilValue);
	CMessageBuffer& operator <<(unsigned long uilValue);

	CMessageBuffer& operator <<(long long iValue);
	CMessageBuffer& operator <<(double dValue);


	//빼기
	CMessageBuffer& operator >>(char& chValue);
	CMessageBuffer& operator >>(unsigned char& uchValue);


	CMessageBuffer& operator >>(short& shValue);
	CMessageBuffer& operator >>(unsigned short& ushValue);

	CMessageBuffer& operator >>(int& iValue);
	CMessageBuffer& operator >>(unsigned int& uiValue);

	CMessageBuffer& operator >>(float& fValue);

	CMessageBuffer& operator >>(long& ilValue);
	CMessageBuffer& operator >>(unsigned long& uilValue);

	CMessageBuffer& operator >>(double& dValue);
	CMessageBuffer& operator >>(long long& iValue);

	int GetData(char* chpDest, int iSize);

	int AddRef();
	void DecRef();

private:
	unsigned char* Buffer;			// 버퍼				

	int ReadPos;										// 읽기 위치
	int WritePos;										// 쓰기 위치
	int BufferSize;										// 버퍼의 크기
	int UseLength;										// 추가된 데이터의 길이

	DWORD* RefCnt;
private:
	enum eMessageBuffer
	{
		eDefault_Buffer_Size = 1024
	};

public:
	static CMemoryPool<CMessageBuffer>* pool; //메모리 풀을 사용하면서 Send Recv TPS가 약 15만씩 증가했음.

	static CMessageBuffer* Alloc();

	static int GetAllocCount();
};