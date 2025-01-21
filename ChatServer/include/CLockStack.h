#pragma once


template <typename T>
class CLockStack
{
public:
	std::optional<T> Pop();
	void Push(T& data);
	void Push(T&& data);

private:
	std::list<T> datas;
	ILock* lockObj;
};

template<typename T>
std::optional<T> CLockStack<T>::Pop()
{	
	lockObj->lock();
	if (datas.empty())
	{
		return std::nullopt;
	}
	T outData = datas.front();
	datas.pop_front();
	lockObj->unlock();
	return outData;
}

template<typename T>
void CLockStack<T>::Push(T& data)
{
	lockObj->lock();	
	datas.push_front(data);
	lockObj->unlock();
}

template<typename T>
void CLockStack<T>::Push(T&& data)
{
	lockObj->lock();	
	datas.push_front(std::move(data));
	lockObj->unlock();
}
