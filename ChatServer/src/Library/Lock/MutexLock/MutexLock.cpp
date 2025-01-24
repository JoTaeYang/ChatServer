#include <mutex>
#include "Interfaces/ILock.h"
#include "MutexLock.h"

void MutexLock::lock()
{
	this->mutex.lock();
}

void MutexLock::unlock()
{
	this->mutex.unlock();
}
