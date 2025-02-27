#pragma once
struct stCHECKCODE
{
	INT64 High;
	INT64 Low;
};

template <typename T>
class CMemoryPool
{
private:
	struct memoryNode
	{
		memoryNode()
		{
			pNext = NULL;
		}
		stCHECKCODE code;
		T data;
		memoryNode* pNext;
	};

	struct TopmemoryNode
	{
		TopmemoryNode()
		{
			pNext = NULL;
			unique = 0;
		}

		memoryNode* pNext;
		__int64 unique;
	};

public:
	CMemoryPool(bool placementNew);

	~CMemoryPool();

	T* Alloc(void);

	bool Free(T** pData);

	int GetAllocCount(void) { return _allocCount; }

	int GetUseCount(void) { return _useCount; }

	void Clear(void);

	//16의 경계에 스는가? 이렇게 선언했을 때.
	//포인터 변수 + int64 변수 있으니까 16byte 경계에 있는 것 아닌가?
	TopmemoryNode* _top;

private:
	memoryNode* CreatememoryNode(void)
	{
		memoryNode* tmp = NULL;

		InterlockedIncrement(&_allocCount);

		if (_placementNew)
		{
			tmp = (memoryNode*)malloc(sizeof(memoryNode));
			tmp->pNext = NULL;
			tmp->code.Low = (INT64)this;
			tmp->code.High = 0;
		}
		else
		{
			tmp = new memoryNode;
			tmp->code.Low = (INT64)this;
			tmp->code.High = 0;
		}
		return tmp;
	}

	long _allocCount;
	long _useCount;

	__int64 uniqueValue;

	bool _placementNew; //생성자 소멸자를 매번 호출할 것인지..
};

template <typename T>
CMemoryPool<T>::CMemoryPool(bool placementNew)
{
	_top = (TopmemoryNode*)_aligned_malloc(sizeof(TopmemoryNode), 16);
	_top->pNext = NULL;
	_top->unique = 0;

	_allocCount = 0;
	_useCount = 0;
	uniqueValue = 0;
	_placementNew = placementNew;
}

template <typename T>
CMemoryPool<T>::~CMemoryPool()
{
	Clear();
}

template <typename T>
T* CMemoryPool<T>::Alloc()
{
	memoryNode* tmp = NULL;
	TopmemoryNode tmpTop;
	InterlockedIncrement(&_useCount);

	__int64 tmpUniq = InterlockedIncrement64(&uniqueValue);
	if (!_top->pNext) //top의 next가 NULL일 때 Pool이 빈 것.
	{
		InterlockedIncrement(&_allocCount);
		if (_placementNew)
		{
			tmp = (memoryNode*)malloc(sizeof(memoryNode));
			tmp->pNext = NULL;
			tmp->code.Low = (INT64)this;
			tmp->code.High = 0;

			return new(&tmp->data) T;
		}
		else
		{
			tmp = new memoryNode;
			tmp->code.Low = (INT64)this;
			tmp->code.High = 0;

			return &tmp->data;
		}
	}
	else //노드가 비지 않았다면 LockFree 방식으로 Pop을 해서 나간다.
	{
		do
		{
			tmpTop.unique = _top->unique;
			tmpTop.pNext = _top->pNext;

			if (!tmpTop.pNext)
			{
				tmp = CreatememoryNode();
				if (_placementNew)
					return new (&tmp->data) T;
				else
					return &tmp->data;
			}
		} while (InterlockedCompareExchange128((long long*)_top, tmpUniq, (long long)tmpTop.pNext->pNext, (long long*)&tmpTop) == 0);

		if (_placementNew)
			return new (&tmpTop.pNext->data) T;

		return &tmpTop.pNext->data;
	}

	return NULL;
}

template <typename T>
bool CMemoryPool<T>::Free(T** pData)
{
	memoryNode* tmp = (memoryNode*)((char*)*pData - sizeof(stCHECKCODE));
	TopmemoryNode tmpTop;

	__int64 tmpUniq = InterlockedIncrement64(&uniqueValue);

	if (tmp->code.Low != (INT64)this)
		return false;

	if (_placementNew)
		tmp->data.~T();

	do
	{
		tmpTop.unique = _top->unique; //현재 TOP이 가진 값이 2개, 지역변수로 선언 후 비교,
		tmpTop.pNext = _top->pNext;
		tmp->pNext = tmpTop.pNext;
	} while (InterlockedCompareExchange128((long long*)_top, tmpUniq, (long long)tmp, (long long*)&tmpTop) == 0);

	InterlockedDecrement(&_useCount);

	*pData = NULL;
	return true;
}

template <typename T>
void CMemoryPool<T>::Clear(void)
{
	memoryNode* storememoryNode = NULL;
	memoryNode* tmp = _top->pNext;
	while (!tmp)
	{
		storememoryNode = tmp->pNext;
		if (_placementNew)
		{
			free(tmp);
		}
		else
		{
			tmp->data.~T();
			delete tmp;
		}
		tmp = storememoryNode;
	}
	_aligned_free(_top);
}