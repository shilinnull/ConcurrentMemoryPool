#pragma once

#include<vector>
#include<iostream>
#include<time.h>
#include<assert.h>

using std::cout;
using std::endl;

// �����ڴ��ǰ4/8���ֽ�
static void*& NextObj(void* obj)
{
	return *(void**)obj;
}

// �����зֺõ�С�������������
class FreeList
{
public:
	void Push(void* obj)
	{
		assert(obj);
		// ͷ��
		NextObj(obj) = _freeList;
		_freeList = obj;
	}
	
	void* Pop()
	{
		assert(_freeList);

		// ͷɾ
		void* obj = _freeList;
		_freeList = NextObj(obj);
		return obj;
	}

	// �ж��Ƿ�Ϊ��
	bool Empty() const
	{
		return _freeList == nullptr;
	}

	
private:
	void* _freeList = nullptr;
};

// ��������ӳ���ϵ
class SizeClass
{
public:

	// ��ȡ���϶������ֽ���
	static size_t RoundUp(size_t bytes)
	{
		if (bytes <= 128)
		{
			return _RoundUp(bytes, 8);
		}
		else if (bytes <= 1024)
		{
			return _RoundUp(bytes, 16);
		}
		else if (bytes <= 8 * 1024)
		{
			return _RoundUp(bytes, 128);
		}
		else if (bytes <= 64 * 1024)
		{
			return _RoundUp(bytes, 1024);
		}
		else if (bytes <= 256 * 1024)
		{
			return _RoundUp(bytes, 8 * 1024);
		}
		else
		{
			assert(false);
			return -1;
		}
	}

	//��ȡ��Ӧ��ϣͰ���±�
	static size_t Index(size_t bytes)
	{
		//ÿ�������ж��ٸ���������
		static size_t groupArray[4] = { 16, 56, 56, 56 };
		if (bytes <= 128)
		{
			return _Index(bytes, 3);
		}
		else if (bytes <= 1024)
		{
			return _Index(bytes - 128, 4) + groupArray[0];
		}
		else if (bytes <= 8 * 1024)
		{
			return _Index(bytes - 1024, 7) + groupArray[0] + groupArray[1];
		}
		else if (bytes <= 64 * 1024)
		{
			return _Index(bytes - 8 * 1024, 10) + groupArray[0] + groupArray[1] + groupArray[2];
		}
		else if (bytes <= 256 * 1024)
		{
			return _Index(bytes - 64 * 1024, 13) + groupArray[0] + groupArray[1] + groupArray[2] + groupArray[3];
		}
		else
		{
			assert(false);
			return -1;
		}
	}
private:
	//һ��д��
	/*static size_t _RoundUp(size_t bytes, size_t alignNum)
	{
		size_t alignSize = 0;
		if (bytes % alignNum != 0)
		{
			alignSize = (bytes / alignNum + 1) * alignNum;
		}
		else
		{
			alignSize = bytes;
		}
		return alignSize;
	}*/

	// λ����д��
	static size_t _RoundUp(size_t bytes, size_t alignNum)
	{
		return ((bytes + alignNum - 1) & ~(alignNum - 1));
	}

	//һ��д��
	/*static size_t _Index(size_t bytes, size_t alignNum)
	{
		size_t index = 0;
		if (bytes % alignNum != 0)
		{
			index = bytes / alignNum;
		}
		else
		{
			index = bytes / alignNum - 1;
		}
		return index;
	}*/

	//λ����д��
	static inline size_t _Index(size_t bytes, size_t alignShift)
	{
		return ((bytes + (1 << alignShift) - 1) >> alignShift) - 1;
	}

};
