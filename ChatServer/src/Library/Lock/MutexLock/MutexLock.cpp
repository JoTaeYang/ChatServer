#include <mutex>
#include "Library/Interface/ILock.h"
#include "Library/Lock/MutexLock/MutexLock.h"

void MutexLock::lock()
{
	this->mutex.lock();
}

void MutexLock::unlock()
{
	this->mutex.unlock();
}
