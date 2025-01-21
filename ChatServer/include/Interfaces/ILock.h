#pragma once

class ILock
{
public:
	virtual void lock() = 0;
	virtual void unlock() = 0;
	virtual ~ILock() = default;
};