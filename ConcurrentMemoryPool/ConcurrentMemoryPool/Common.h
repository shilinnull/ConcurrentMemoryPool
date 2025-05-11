#pragma once

#include<vector>
#include<iostream>
#include<time.h>
#include<assert.h>

using std::cout;
using std::endl;

// 访问内存的前4/8个字节
static void*& NextObj(void* obj)
{
	return *(void**)obj;
}

// 管理切分好的小对象的自由链表
class FreeList
{
public:
	void Push(void* obj)
	{
		assert(obj);
		// 头插
		NextObj(obj) = _freeList;
		_freeList = obj;
	}
	
	void* Pop()
	{
		assert(_freeList);

		// 头删
		void* obj = _freeList;
		_freeList = NextObj(obj);
		return obj;
	}

	// 判断是否为空
	bool Empty() const
	{
		return _freeList == nullptr;
	}

	
private:
	void* _freeList = nullptr;
};

// 管理对其和映射关系
class SizeClass
{
public:

	// 获取向上对齐后的字节数
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

	//获取对应哈希桶的下标
	static size_t Index(size_t bytes)
	{
		//每个区间有多少个自由链表
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
	//一般写法
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

	// 位运算写法
	static size_t _RoundUp(size_t bytes, size_t alignNum)
	{
		return ((bytes + alignNum - 1) & ~(alignNum - 1));
	}

	//一般写法
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

	//位运算写法
	static inline size_t _Index(size_t bytes, size_t alignShift)
	{
		return ((bytes + (1 << alignShift) - 1) >> alignShift) - 1;
	}

};
