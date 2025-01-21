#pragma once


class SpinLock : public ILock
{
public:
	void lock() override;
	void unlock() override;

private:
	std::atomic_flag flag = ATOMIC_FLAG_INIT;
};