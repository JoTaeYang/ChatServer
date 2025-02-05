#pragma once

#include "Library/Interface/ILock.h"
#include <mutex>


class MutexLock : public ILock
{

public:
	void lock() override;
	void unlock() override;

private:
	std::mutex mutex;
};