#pragma once

#include "Common.h"
#include "ThreadCache.h"

static void* ConcurrentAlloc(size_t size)
{
	// 通过TLS 每个线程无锁的获取自己的专属的ThreadCache对象
	if (pTLSThreadCache == nullptr)
	{
		pTLSThreadCache = new ThreadCache;
	}
	cout << std::this_thread::get_id() << ":" << &pTLSThreadCache << endl;
	
	// 返回分配的地址
	return pTLSThreadCache->Allocate(size);
}


static void ConcurrentFree(void* ptr, size_t size)
{
	assert(ptr);
	pTLSThreadCache->Deallocate(ptr, size);
}
