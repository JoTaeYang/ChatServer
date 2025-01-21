#pragma once

class MutexLock : public ILock
{

public:
	void lock() override;
	void unlock() override;

private:
	std::mutex mutex;
};