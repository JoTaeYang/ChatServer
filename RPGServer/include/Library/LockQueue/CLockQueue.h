#pragma once

class ILock;

#include <memory>
#include <atomic>


template <typename T>
class CLockQueue
{
private:
	struct Node 
	{
		Node()
		{
			next = NULL;
		}

		T value;
		Node* next;
	};

public:
	CLockQueue() = delete;
	CLockQueue(ILock* InLockObj);
	~CLockQueue();

	bool Dequeue(T& OutData);
	void Enqueue(T& Data);
	void Enqueue(T&& Data);
	int Peek(T& OutData, int _pos);

	void TestSetLockObj(ILock* InLockObj);
	int GetCount();
private:
	alignas(64) ILock* lockObj;

	Node* head;
	Node* tail;

	std::atomic<int> count;
};

template<typename T>
CLockQueue<T>::CLockQueue(ILock* InLockObj)
{
	head = new Node;
	tail = new Node;
	lockObj = InLockObj;

	head->next = new Node;
	tail->next = head->next;
	count = 0;
}

template<typename T>
CLockQueue<T>::~CLockQueue()
{
	delete lockObj;
}

template<typename T>
bool CLockQueue<T>::Dequeue(T& OutData)
{
	if (count.fetch_sub(1) <= 0)
	{
		count.fetch_add(1);
		return false;
	}
		
	lockObj->lock();
	Node* removeNode = head->next;
	Node* outNode = head->next->next;
	OutData = outNode->value;
	head->next = outNode;
	delete removeNode;
	lockObj->unlock();
	return true;
}

template<typename T>
void CLockQueue<T>::Enqueue(T& Data)
{
	Node* newNode = new Node;
	newNode->value = Data;
	lockObj->lock();
	tail->next->next = newNode;	
	tail->next = newNode;
	lockObj->unlock();
	count.fetch_add(1);
}

template<typename T>
void CLockQueue<T>::Enqueue(T&& Data)
{
	Node* newNode = new Node;
	newNode->value = std::move(Data);
	lockObj->lock();
	tail->next->next = newNode;
	tail->next = newNode;
	lockObj->unlock();
}

template<typename T>
inline int CLockQueue<T>::Peek(T& OutData, int _pos)
{
	if (_pos > count)
		return -1;
	
	Node* tmpHead = head->next->next;
	for (int i = 0; i < _pos; ++i)
	{
		if (tmpHead == NULL || tmpHead == tail->next)
			return -1;
		tmpHead = tmpHead->next;
	}

	if (tmpHead == NULL)
		return -1;

	OutData = tmpHead->value;

	return _pos;
}

template<typename T>
void CLockQueue<T>::TestSetLockObj(ILock* InLockObj)
{
	if (lockObj != NULL)
		delete lockObj;
	lockObj = InLockObj;
}

template<typename T>
inline int CLockQueue<T>::GetCount()
{
	return count;
}

