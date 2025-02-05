#pragma once

#include <Windows.h>
/*
* @struct Header
* @brief Represent Library Header
* 
*/
struct Header
{	
	//@brief ��Ŷ�� Ÿ�� 2byte
	WORD wType;

	//@brief ��Ŷ�� ������ 1byte
	BYTE bySize;		

	//@brief ��Ŷ�� �ڵ�. 0x89
	BYTE byCode;		
};