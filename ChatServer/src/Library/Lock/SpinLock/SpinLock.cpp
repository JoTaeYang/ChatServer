#include <atomic>
#include "Library/Interface/ILock.h"
#include "Library/Lock/SpinLock/SpinLock.h"

void SpinLock::lock()
{
	while(flag.test_and_set(std::memory_order_acquire)){}
}

void SpinLock::unlock()
{
	flag.clear(std::memory_order_release);
}
