#include "Common.h"
#include "ThreadCache.h"


void* ThreadCache::FetchFromCentralCache(size_t index, size_t size)
{
	// ... 后面来写
	return nullptr;
}

void* ThreadCache::Allocate(size_t size)
{
	assert(size <= MAX_BYTES);
	// 获取对齐后的字节数
	size_t alignSize = SizeClass::RoundUp(size);
	// 获取自由链表的哈希桶下标
	size_t index = SizeClass::Index(size);
	// 
	if (!_freeLists[index].Empty())
	{
		return _freeLists[index].Pop();
	}
	else
	{
		// 再从下层获取
		return FetchFromCentralCache(index, alignSize);
	}
}

void ThreadCache::Deallocate(void* ptr, size_t size)
{
	assert(ptr);
	assert(size >= MAX_BYTES);

	// 找对映射的自由链表桶，对象插入进入
	size_t index = SizeClass::Index(size);
	_freeLists[index].Push(ptr);
}
