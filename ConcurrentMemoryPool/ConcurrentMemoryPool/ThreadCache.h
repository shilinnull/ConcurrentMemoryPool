#pragma once
#include "Common.h"
#include <thread>

// 小于等于MAX_BYTES，就找thread cache申请
// 大于MAX_BYTES，就直接找page cache或者系统堆申请
static const size_t MAX_BYTES = 256 * 1024;
//thread cache和central cache自由链表哈希桶的表大小
static const size_t NFREELISTS = 208;

class ThreadCache
{
public:
	// 分配内存对象
	void* Allocate(size_t size);

	// 释放
	void Deallocate(void* ptr, size_t size);

	// 从下层获取
	void* FetchFromCentralCache(size_t index, size_t size);
private:
	FreeList _freeLists[NFREELISTS]; // 自由链表哈希桶
};

//TLS - Thread Local Storage
static _declspec(thread) ThreadCache* pTLSThreadCache = nullptr;

