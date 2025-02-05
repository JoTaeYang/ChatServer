#include "Library/RingBuffer/RingBuffer.h"

#include <iostream>
#include <Windows.h>
#include <strsafe.h>
#include <time.h>


CRingBuffer::CRingBuffer()
{
	_buffer = new char[e_DEFAULT_SIZE];
	_iFrontPos = 0;
	_iRearPos = 0;
}
CRingBuffer::~CRingBuffer()
{
	_iFrontPos = 0;
	_iRearPos = 0;
	delete[] _buffer;
}

int CRingBuffer::GetBufferSize(void)
{
	return e_DEFAULT_SIZE;
}

int CRingBuffer::GetUseSize(void)
{
	int tmpRear = _iRearPos;
	int tmpFront = _iFrontPos;
	if (tmpRear >= tmpFront)
		return tmpRear - tmpFront;
	else
		return (e_DEFAULT_SIZE - tmpFront) + tmpRear;
}

int CRingBuffer::GetFreeSize(void)
{
	if (_iFrontPos > _iRearPos)
		return _iFrontPos - _iRearPos;
	else
		return e_DEFAULT_SIZE - (_iRearPos - _iFrontPos);

}

int CRingBuffer::DirectEnqueueSize(void)
{
	int tmpRear = _iRearPos;
	int tmpFront = _iFrontPos;
	if (tmpRear >= tmpFront)
		return (e_DEFAULT_SIZE - tmpRear);
	else
		return tmpFront - tmpRear;
}

int CRingBuffer::DirectDequeueSize(void)
{
	int tmpRear = _iRearPos;
	int tmpFront = _iFrontPos;
	if (tmpRear >= tmpFront)
		return _iRearPos - _iFrontPos;
	else
		return e_DEFAULT_SIZE - _iFrontPos;
}

//enqueue를 하는데 공간이 없다? 그러면 바로 로그 찍어서 남겨보기.
int CRingBuffer::Enqueue(char* chpData, int iSize)
{
	int ret_val = 0;
	int tmp_RearPos = _iRearPos;
	int tmp_FrontPos = _iFrontPos;

	while (iSize > 0)
	{
		if ((tmp_RearPos + 1) % e_DEFAULT_SIZE == tmp_FrontPos)
		{
			break;
		}

		_buffer[tmp_RearPos] = *chpData;
		tmp_RearPos = (tmp_RearPos + 1) % e_DEFAULT_SIZE;

		++chpData;
		iSize--;
		++ret_val;
	}
	_iRearPos = tmp_RearPos;
	return ret_val;
}
//dequeue는 뽑을 수 있는 크기만큼 만 뽑는다.
int CRingBuffer::Dequeue(char* chpDest, int iSize)
{
	int tmpPos = _iFrontPos;
	int tmpRearPos = _iRearPos;
	int count = 0;

	if (_iFrontPos == _iRearPos) //비었을 때
		return 0;

	while (count < iSize &&
		tmpPos != tmpRearPos)
	{
		chpDest[count] = _buffer[tmpPos];
		tmpPos = (tmpPos + 1) % e_DEFAULT_SIZE;
		++count;
	}

	_iFrontPos = tmpPos;
	return count;
}

int CRingBuffer::Peek(char* chpDest, int iSize)
{	
	int tmpPos = _iFrontPos;
	int tmpRearPos = _iRearPos;
	int count = 0;
		
	if (_iFrontPos == _iRearPos) //비었을 때
		return 0;

	while (count < iSize &&
		tmpPos != tmpRearPos)
	{	
		chpDest[count] = _buffer[tmpPos];
		tmpPos = (tmpPos + 1) % e_DEFAULT_SIZE;
		++count;
	}

	return count;
}


char* CRingBuffer::GetFrontBufferPtr(void)
{
	return &_buffer[_iFrontPos];
}
char* CRingBuffer::GetRearBufferPtr(void)
{
	return &_buffer[_iRearPos];
}

char* CRingBuffer::GetStartBufferPtr(void)
{
	return _buffer;
}

int CRingBuffer::MoveRear(int iSize)
{
	int tmpRear = _iRearPos;
	tmpRear = (tmpRear + iSize) % e_DEFAULT_SIZE;

	_iRearPos = tmpRear;
	return _iRearPos;
}

int CRingBuffer::MoveFront(int iSize)
{
	int tmpFront = _iFrontPos;
	_iFrontPos = (_iFrontPos + iSize) % e_DEFAULT_SIZE;

	//_iFrontPos = tmpFront;
	return _iFrontPos;
}

void CRingBuffer::Clear()
{
	_iFrontPos = 0;
	_iRearPos = 0;
}

int CRingBuffer::GetTestRearPoint()
{
	return _iRearPos;
}

int CRingBuffer::GetTestFrontPoint()
{
	return _iFrontPos;
}