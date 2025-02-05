#include <ecs/pool_allocator.hpp>

PoolAllocator::PoolAllocator(size_t blockSize, size_t blockCount) :
    _blockSize {blockSize},
    // Space for both blocks and free list
    _memory {(byte*)::operator new((blockSize + sizeof(_FreeBlock)) * blockCount)},
    // Initialize _headFreeBlock and _freeList at the correct position in _memory
    _freeList {(_FreeBlock*)(_memory + (blockSize * blockCount))},
    _headFreeBlock {new (_freeList) _FreeBlock {0, nullptr}}
{
    // initialize the free list
    for(size_t blockIdx {0}; blockIdx <= blockCount - 2; blockIdx++)
    {
        _FreeBlock* currentBlock {_freeList + blockIdx};
        currentBlock->next = new (currentBlock + 1) _FreeBlock {blockIdx + 1, nullptr};
    }
}

PoolAllocator::~PoolAllocator()
{
    ::operator delete(_memory);
}

PoolAllocator::byte* PoolAllocator::allocate()
{
    // When there are no more blocks to allocate
    if(_headFreeBlock == nullptr)
    {
        throw std::bad_alloc {};
    }

    // "Remove" the first available free block (by just repointing _headFreeBlock) 
    // from free list and update _headFreeBlock
    byte* ret {_memory + (_headFreeBlock->blockIndex * _blockSize)};
    _headFreeBlock = _headFreeBlock->next;
    return ret;
}

// Undefined behaviour if block wasn't allocated from this pool
void PoolAllocator::deallocate(byte* block)
{
    size_t blockIdx {(size_t)(block - _memory) / _blockSize};
    _FreeBlock* newFreeBlock {_freeList + blockIdx};

    // Add the new free block to head of free list
    newFreeBlock->next = _headFreeBlock;
    _headFreeBlock = newFreeBlock;
}