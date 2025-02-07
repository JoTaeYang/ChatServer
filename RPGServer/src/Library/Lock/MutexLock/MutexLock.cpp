#include "Library/Lock/MutexLock/MutexLock.h"

void MutexLock::lock()
{
	this->mutex.lock();
}

void MutexLock::unlock()
{
	this->mutex.unlock();
}
