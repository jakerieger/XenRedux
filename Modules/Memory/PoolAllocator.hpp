// Author: Jake Rieger
// Created: 12/16/2024.
//

#pragma once

#include <cstddef>

namespace x::Memory {
    /// @brief Basic implementation of a pool allocator. Provides O(1) alloc/dealloc.
    ///
    /// TODO: Consider re-implementing this using templates
    class PoolAllocator {
    public:
        PoolAllocator(size_t objectSize, size_t capacity);
        ~PoolAllocator();

        void* allocate();
        void deallocate(void* ptr);
        [[nodiscard]] size_t getCapacity() const;
        [[nodiscard]] size_t getObjectSize() const;

    private:
        void* _memory;        // start of memory block
        void* _freeListHead;  // ptr to the first available slot
        size_t _objectSize;
        size_t _capacity;
    };
}  // namespace x::Memory