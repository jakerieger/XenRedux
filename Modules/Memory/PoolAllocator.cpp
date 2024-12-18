// Author: Jake Rieger
// Created: 12/16/2024.
//

#include "PoolAllocator.hpp"
#include "Types.hpp"

#include <cassert>
#include <new>

namespace x::Memory {
    PoolAllocator::PoolAllocator(size_t objectSize, size_t capacity)
        : _memory(nullptr), _freeListHead(nullptr), _objectSize(objectSize), _capacity(capacity) {
        assert(_objectSize >= sizeof(void*) &&
               "Object size must be at least the size of a pointer!");
        // align obj size to at least ptr size
        _objectSize = (_objectSize + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
        _memory     = ::operator new(_objectSize * _capacity);  // allocate our entire block upfront
        _freeListHead = _memory;
        auto current  = CAST<void**>(_freeListHead);
        // this nightmare is basically just initializing all the memory in our block one chunk of
        // size T at a time.
        for (size_t i = 0; i < _capacity - 1; ++i) {
            *current = CAST<void*>(CAST<u8*>(_memory) + (i + 1) * _objectSize);
            current  = CAST<void**>(*current);  // set current to the next free chunk in the block
        }
        *current = nullptr;  // last slot points to null
    }

    PoolAllocator::~PoolAllocator() {
        ::operator delete(_memory);
    }

    void* PoolAllocator::allocate() {
        if (_freeListHead == nullptr) { throw std::bad_alloc(); }
        void* slot    = _freeListHead;
        _freeListHead = *CAST<void**>(_freeListHead);
        return slot;
    }

    void PoolAllocator::deallocate(void* ptr) {
        assert(ptr >= _memory && ptr < CAST<u8*>(_memory) + _objectSize * _capacity &&
               "Pointer out of pool bounds!");
        *CAST<void**>(ptr) = _freeListHead;
        _freeListHead      = ptr;
    }

    size_t PoolAllocator::getCapacity() const {
        return _capacity;
    }

    size_t PoolAllocator::getObjectSize() const {
        return _objectSize;
    }
}  // namespace x::Memory