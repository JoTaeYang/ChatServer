
#include <WinSock2.h>
#include "CSession.h"

void CSession::Init()
{
	this->_socket = 0;
	ZeroMemory(this->buffer, sizeof(this->buffer));
	this->index = -1;
}
