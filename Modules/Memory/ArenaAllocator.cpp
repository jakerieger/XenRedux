// Author: Jake Rieger
// Created: 12/16/2024.
//

#include "ArenaAllocator.hpp"
#include "Panic.hpp"
#include "Types.hpp"

#include <cassert>
#include <new>

namespace x::Memory {
    ArenaAllocator::ArenaAllocator(size_t totalSize) : _totalSize(totalSize), _offset(0) {
        _memory = ::operator new(_totalSize);
    }

    ArenaAllocator::~ArenaAllocator() {
        ::operator delete(_memory);
    }

    void* ArenaAllocator::allocate(size_t size, size_t alignment) {
        if (size <= 0) { Panic("allocate() called with invalid size (<= 0)!"); }
        const size_t alignedOffset = _align(_offset, alignment);
        if (alignedOffset + size > _totalSize) { Panic("Out of memory!"); }
        void* ptr = CAST<u8*>(_memory) + alignedOffset;
        _offset += alignedOffset + size;
        return ptr;
    }

    void ArenaAllocator::reset() {
        _offset = 0;
    }

    size_t ArenaAllocator::getOffset() const {
        return _offset;
    }

    size_t ArenaAllocator::getTotalSize() const {
        return _totalSize;
    }

    size_t ArenaAllocator::_align(size_t offset, size_t alignment) {
        const size_t alignedOffset = (offset + alignment - 1) & ~(alignment - 1);
        return alignedOffset;
    }
}  // namespace x::Memory