#pragma once

#include "Common.h"

#ifdef _WIN32
#include <Windows.h>
#else
//...
#endif

// ֱ��ȥ�������밴ҳ����ռ�
inline static void* SystemAlloc(size_t kpage)
{
#ifdef _WIN32
    void* ptr = VirtualAlloc(0, kpage << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    // linux��brk mmap��
#endif
    if (ptr == nullptr)
        throw std::bad_alloc();
    return ptr;
}

// �����ڴ��
template<class T>
class ObjectPool
{
public:
    T* New()
    {
        T* obj = nullptr;

        // ����ʹ�û��������ڴ�飬ֱ��ȡһ��
        if (_freeList)
        {
            // �������������ͷɾ
            obj = (T*)_freeList; // ��ͷת����objָ��Ķ���

             // ������ȡ����ͷ4���ֽڻ���8���ֽڵĵ�ַ��Ҳ������һ���ڴ��ĵ�ַ
            _freeList = *((void**)_freeList); // �����þ���ȡ��һ��ָ��Ĵ�С
        }
        else // ���ռ�
        {
            // ��Ҫ��֤������Դ���µ�ַ
            size_t objSize = sizeof(T) < sizeof(void*) ? sizeof(void*) : sizeof(T);
            // ���ʣ����ֽ���С��objSize������
            if (_leftBytes < objSize)
            {
                _leftBytes = 1 << 17; // �൱�� 128 * 1024
                //_memory = (char*)malloc(_leftBytes);    // ʹ��malloc
                _memory = (char*)SystemAlloc(_leftBytes >> 13); // ֱ����ϵͳ����ռ�

                // ���ռ�ʧ���ˣ���Ҫ�������쳣
                if (!_memory)
                    throw std::bad_alloc();
            }
            
            obj = (T*)_memory; // ��objһ��ռ�

            // �ڴ��ֻ��ǰ�ߣ���������
            _memory += objSize; // _memory������
            _leftBytes -= objSize; // ����ʣ���С
        } // allloc end...

        // ��ʼ���ڴ棬ʹ�ö�λnew
        new(obj)T;

        // ���ظ��ϲ�
        return obj;
    }

    void Delete(T* obj)
    {
        // ��������������������
        obj->~T();

        /////////////ͷ�嵽��������///////////
        // ����������ָ��objͷ4���ֽڻ���8���ֽڣ�Ҳ���൱����obj��������������ͷ
        *((void**)obj) = _freeList; 
        _freeList = obj;
    }

private:
    char* _memory = nullptr;   // ָ�����ڴ��ָ��
    void* _freeList = nullptr; // �������������ӵ���������
    size_t _leftBytes = 0;     // ����ڴ����зֹ�����ʣ���ֽ���
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
    // �����ͷŵ��ִ�
    const size_t Rounds = 3;
    // ÿ�������ͷŶ��ٴ�
    const size_t N = 1000000;
    std::vector<TreeNode*> v1;
    v1.reserve(N);

    //malloc��free
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

    //�����ڴ��
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
