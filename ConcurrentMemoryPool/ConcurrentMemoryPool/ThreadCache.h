#pragma once
#include "Common.h"
#include <thread>

// С�ڵ���MAX_BYTES������thread cache����
// ����MAX_BYTES����ֱ����page cache����ϵͳ������
static const size_t MAX_BYTES = 256 * 1024;
//thread cache��central cache���������ϣͰ�ı��С
static const size_t NFREELISTS = 208;

class ThreadCache
{
public:
	// �����ڴ����
	void* Allocate(size_t size);

	// �ͷ�
	void Deallocate(void* ptr, size_t size);

	// ���²��ȡ
	void* FetchFromCentralCache(size_t index, size_t size);
private:
	FreeList _freeLists[NFREELISTS]; // ���������ϣͰ
};

//TLS - Thread Local Storage
static _declspec(thread) ThreadCache* pTLSThreadCache = nullptr;

