#pragma once

#include <memory>
#include <vector>

class CMessageBuffer
{
public:
	CMessageBuffer();
	CMessageBuffer(int InBufferSize);
	~CMessageBuffer();

	void Clear(void);

public:
	int PutData(char* chpSrc, int iSrcSize);

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

private:
	unsigned char* Buffer;			// 버퍼				

	int ReadPos;										// 읽기 위치
	int WritePos;										// 쓰기 위치
	int BufferSize;										// 버퍼의 크기
	int UseLength;										// 추가된 데이터의 길이

private:
	enum eMessageBuffer
	{
		eDefault_Buffer_Size = 1024
	};
};