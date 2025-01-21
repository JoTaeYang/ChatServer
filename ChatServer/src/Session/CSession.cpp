
#include <WinSock2.h>
#include "CSession.h"

CSession::CSession() : _socket(0), index(0), buffer{0}
{

}

void CSession::Init()
{
	this->_socket = 0;
	ZeroMemory(this->buffer, sizeof(this->buffer));
	this->index = -1;
}

