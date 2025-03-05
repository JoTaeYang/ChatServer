#pragma once


#include "Library/Interface/ILock.h"
#include <atomic>

class SpinLock : public ILock
{
public:
	~SpinLock();
	void lock() override;
	void unlock() override;

private:
	std::atomic_flag flag;
};