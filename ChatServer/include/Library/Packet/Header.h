#pragma once

#include <Windows.h>
/*
* @struct Header
* @brief Represent Library Header
* 
*/
struct Header
{	
	//@brief 패킷의 타입 2byte
	WORD wType;

	//@brief 패킷의 사이즈 1byte
	BYTE bySize;		

	//@brief 패킷의 코드. 0x89
	BYTE byCode;		
};