#include "Library/MessageBuffer/MessageBuffer.h"
#include "Library/MemoryPool/CMemoryPool.h"
#include <Windows.h>

CMessageBuffer::CMessageBuffer()
{
	Buffer = new unsigned char[eMessageBuffer::eDefault_Buffer_Size] {0};

	ReadPos = 0;
	WritePos = 0;
	BufferSize = eDefault_Buffer_Size;
	UseLength = 0;

	RefCnt = new DWORD;

	*RefCnt = 0;
}

CMessageBuffer::CMessageBuffer(int InBufferSize)
{	
	Buffer = new unsigned char[InBufferSize] {0};


	ReadPos = 0;
	WritePos = 0;
	BufferSize = InBufferSize;
	UseLength = 0;

	RefCnt = new DWORD;

	*RefCnt = 0;
}

CMessageBuffer::~CMessageBuffer()
{
	if (Buffer != NULL)
		delete[] Buffer;

	if (RefCnt != NULL)
		delete RefCnt;

	Buffer = NULL;
}

void CMessageBuffer::Clear(void)
{
	ReadPos = 0;
	WritePos = 0;
	UseLength = 0;
}

int CMessageBuffer::PutData(char* chpSrc, int iSrcSize)
{
	memcpy(Buffer + WritePos, chpSrc, iSrcSize);
	WritePos += iSrcSize;
	UseLength += iSrcSize;
	return iSrcSize;
}

unsigned char* CMessageBuffer::GetBufferPtr(void)
{
	return Buffer;
}

/*
³Ö±â
*/
CMessageBuffer& CMessageBuffer::operator <<(char chValue)
{
	*(Buffer + WritePos) = chValue;
	WritePos += 1;
	UseLength += 1;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(unsigned char uchValue)
{
	*(Buffer + WritePos) = uchValue;
	WritePos += 1;
	UseLength += 1;
	return *this;
}


CMessageBuffer& CMessageBuffer::operator <<(short shValue)
{
	memcpy_s((Buffer + WritePos), BufferSize, &shValue, 2);
	WritePos += 2;
	UseLength += 2;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(unsigned short ushValue)
{
	memcpy_s((Buffer + WritePos), BufferSize, &ushValue, 2);
	WritePos += 2;
	UseLength += 2;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(int iValue)
{
	//memcpy_s((Buffer + WritePos), BufferSize,  &iValue, 4 );
	Buffer[WritePos++] = iValue;
	Buffer[WritePos++] = iValue >> 8;
	Buffer[WritePos++] = iValue >> 16;
	Buffer[WritePos++] = iValue >> 24;
	UseLength += 4;
	//WritePos += 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(unsigned int uiValue)
{
	memcpy_s((Buffer + WritePos), BufferSize, &uiValue, 4);
	WritePos += 4;
	UseLength += 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(float fValue)
{
	memcpy_s((Buffer + WritePos), BufferSize, &fValue, 4);
	WritePos += 4;
	UseLength += 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(long ilValue)
{
	memcpy_s((Buffer + WritePos), BufferSize, &ilValue, 4);
	WritePos += 4;
	UseLength += 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(unsigned long uilValue)
{
	memcpy_s((Buffer + WritePos), BufferSize, &uilValue, 4);
	WritePos += 4;
	UseLength += 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(double dValue)
{
	memcpy_s((Buffer + WritePos), BufferSize, &dValue, 8);
	WritePos += 8;
	UseLength += 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator <<(long long iValue)
{
	memcpy_s((Buffer + WritePos), BufferSize, &iValue, 8);
	WritePos += 8;
	UseLength += 8;
	return *this;
}

/*
»©±â
*/


CMessageBuffer& CMessageBuffer::operator>>(char& chValue)
{
	chValue = *(Buffer + ReadPos);
	ReadPos += 1;
	UseLength -= 1;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(unsigned char& uchValue)
{
	uchValue = *(Buffer + ReadPos);
	ReadPos += 1;
	UseLength -= 1;
	return *this;
}


CMessageBuffer& CMessageBuffer::operator >>(short& shValue)
{
	shValue = *(Buffer + ReadPos) + (*(Buffer + (ReadPos + 1)) << 8);
	ReadPos += 2;
	UseLength -= 2;
	return *this;
}
CMessageBuffer& CMessageBuffer::operator >>(unsigned short& ushValue)
{
	ushValue = *(Buffer + ReadPos) + (*(Buffer + (ReadPos + 1)) << 8);
	ReadPos += 2;
	UseLength -= 2;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(int& iValue)
{
	iValue = *(Buffer + ReadPos) + (*(Buffer + (ReadPos + 1)) << 8) +
		(*(Buffer + (ReadPos + 2)) << 16) + (*(Buffer + (ReadPos + 3)) << 24);
	ReadPos += 4;
	UseLength -= 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(unsigned int& uiValue)
{
	uiValue = *(Buffer + ReadPos) + (*(Buffer + (ReadPos + 1)) << 8) +
		(*(Buffer + (ReadPos + 2)) << 16) + (*(Buffer + (ReadPos + 3)) << 24);
	ReadPos += 4;
	UseLength -= 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(float& fValue)
{
	memcpy(&(fValue), (Buffer + ReadPos), 4);
	ReadPos += 4;
	UseLength -= 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(long& ilValue)
{
	ilValue = *(Buffer + ReadPos) + (*(Buffer + (ReadPos + 1)) << 8) +
		(*(Buffer + (ReadPos + 2)) << 16) + (*(Buffer + (ReadPos + 3)) << 24);
	ReadPos += 4;
	UseLength -= 4;

	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(unsigned long& uilValue)
{
	uilValue = *(Buffer + ReadPos) + (*(Buffer + (ReadPos + 1)) << 8) +
		(*(Buffer + (ReadPos + 2)) << 16) + (*(Buffer + (ReadPos + 3)) << 24);
	ReadPos += 4;
	UseLength -= 4;
	return *this;
}

CMessageBuffer& CMessageBuffer::operator >>(double& dValue)
{
	memcpy(&(dValue), (Buffer + ReadPos), 8);
	ReadPos += 8;
	UseLength -= 8;
	return *this;
}
CMessageBuffer& CMessageBuffer::operator >>(long long& iValue)
{
	memcpy(&(iValue), (Buffer + ReadPos), 8);
	ReadPos += 8;
	UseLength -= 8;
	return *this;
}

int CMessageBuffer::AddRef()
{
	return InterlockedIncrement(RefCnt);
}

void CMessageBuffer::DecRef()
{
	CMessageBuffer* p = this;

	if (0 == InterlockedDecrement(RefCnt))
	{
		pool->Free(&p);
	}
}

CMessageBuffer* CMessageBuffer::Alloc()
{
	CMessageBuffer* tmp = pool->Alloc();

	tmp->Clear();
	tmp->AddRef();

	return tmp;
}

int CMessageBuffer::GetAllocCount()
{
	return 0;
}
