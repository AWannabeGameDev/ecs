#ifndef POOL_ALLOC_HPP
#define POOL_ALLOC_HPP

#include <cstddef>
#include <new>
#include <stdexcept>

class PoolAllocator
{
private :
    using byte = std::byte;
    using size_t = std::size_t;

    struct _FreeBlock
    {
        size_t blockIndex;
        _FreeBlock* next;
    };

    size_t _blockSize;

    byte* const _memory; // Ptr. to start of memory pool
    _FreeBlock* const _freeList; // Ptr. to start of free list
    _FreeBlock* _headFreeBlock; // Ptr. to first available free block

public :
    PoolAllocator(size_t blockSize, size_t blockCount);

    ~PoolAllocator();

    // Don't want to copy/move a pool allocator's pool
    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator& operator=(const PoolAllocator&) = delete;

    // Only allocates, construction is caller's responsibility
    byte* allocate();

    // Only deallocates, destruction (via explicit call to destructor) is caller's responsibility
    void deallocate(byte* block);
};

#endif