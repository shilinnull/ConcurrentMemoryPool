#pragma once

#include "Common.h"

#ifdef _WIN32
#include <Windows.h>
#else
//...
#endif

// 直接去堆上申请按页申请空间
inline static void* SystemAlloc(size_t kpage)
{
#ifdef _WIN32
    void* ptr = VirtualAlloc(0, kpage << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    // linux下brk mmap等
#endif
    if (ptr == nullptr)
        throw std::bad_alloc();
    return ptr;
}

// 定长内存池
template<class T>
class ObjectPool
{
public:
    T* New()
    {
        T* obj = nullptr;

        // 优先使用还回来的内存块，直接取一块
        if (_freeList)
        {
            // 从自由链表进行头删
            obj = (T*)_freeList; // 将头转换成obj指针的对象

             // 这里是取的是头4个字节或者8个字节的地址，也就是下一个内存块的地址
            _freeList = *((void**)_freeList); // 解引用就是取的一个指针的大小
        }
        else // 开空间
        {
            // 需要保证对象可以存放下地址
            size_t objSize = sizeof(T) < sizeof(void*) ? sizeof(void*) : sizeof(T);
            // 如果剩余的字节数小于objSize就扩容
            if (_leftBytes < objSize)
            {
                _leftBytes = 1 << 17; // 相当于 128 * 1024
                //_memory = (char*)malloc(_leftBytes);    // 使用malloc
                _memory = (char*)SystemAlloc(_leftBytes >> 13); // 直接向系统申请空间

                // 开空间失败了，需要进行抛异常
                if (!_memory)
                    throw std::bad_alloc();
            }
            
            obj = (T*)_memory; // 给obj一块空间

            // 内存池只往前走，不往后退
            _memory += objSize; // _memory往后走
            _leftBytes -= objSize; // 更新剩余大小
        } // allloc end...

        // 初始化内存，使用定位new
        new(obj)T;

        // 返回给上层
        return obj;
    }

    void Delete(T* obj)
    {
        // 调用析构函数进行清理
        obj->~T();

        /////////////头插到自由链表///////////
        // 让自由链表指向obj头4个字节或者8个字节，也就相当于让obj变成了自由链表的头
        *((void**)obj) = _freeList; 
        _freeList = obj;
    }

private:
    char* _memory = nullptr;   // 指向大块内存的指针
    void* _freeList = nullptr; // 还回来过程链接的自由链表
    size_t _leftBytes = 0;     // 大块内存在切分过程中剩余字节数
};
struct TreeNode
{
    int _val;
    TreeNode* _left;
    TreeNode* _right;
    TreeNode()
        :_val(0)
        , _left(nullptr)
        , _right(nullptr)
    {}
};

void TestObjectPool()
{
    // 申请释放的轮次
    const size_t Rounds = 3;
    // 每轮申请释放多少次
    const size_t N = 1000000;
    std::vector<TreeNode*> v1;
    v1.reserve(N);

    //malloc和free
    size_t begin1 = clock();
    for (size_t j = 0; j < Rounds; ++j)
    {
        for (int i = 0; i < N; ++i)
        {
            v1.push_back(new TreeNode);
        }
        for (int i = 0; i < N; ++i)
        {
            delete v1[i];
        }
        v1.clear();
    }
    size_t end1 = clock();

    //定长内存池
    ObjectPool<TreeNode> TNPool;
    std::vector<TreeNode*> v2;
    v2.reserve(N);
    size_t begin2 = clock();
    for (size_t j = 0; j < Rounds; ++j)
    {
        for (int i = 0; i < N; ++i)
        {
            v2.push_back(TNPool.New());
        }
        for (int i = 0; i < N; ++i)
        {
            TNPool.Delete(v2[i]);
        }
        v2.clear();
    }
    size_t end2 = clock();

    cout << "new cost time:" << end1 - begin1 << endl;
    cout << "object pool cost time:" << end2 - begin2 << endl;
}
