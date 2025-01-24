#pragma once


template <typename T>
class CLockStack
{
public:
	CLockStack() = delete;
	CLockStack(ILock* InLockObj);
	~CLockStack();

	std::optional<T> Pop();
	void Push(T& data);
	void Push(T&& data);

private:
	std::vector<T> datas;
	ILock* lockObj;
};

template<typename T>
CLockStack<T>::CLockStack(ILock* InLockObj)
{
	lockObj = InLockObj;
	datas.reserve(2000);
}

 template<typename T>
CLockStack<T>::~CLockStack()
{
	delete lockObj;
}

template<typename T>
std::optional<T> CLockStack<T>::Pop()
{	
	lockObj->lock();
	if (datas.empty())
	{
		return std::nullopt;
	}
	T outData = datas.back();
	datas.pop_back();
	lockObj->unlock();
	return outData;
}

template<typename T>
void CLockStack<T>::Push(T& data)
{
	lockObj->lock();	
	datas.emplace_back(data);
	lockObj->unlock();
}

template<typename T>
void CLockStack<T>::Push(T&& data)
{
	lockObj->lock();	
	datas.emplace_back(std::move(data));	
	lockObj->unlock();
}
