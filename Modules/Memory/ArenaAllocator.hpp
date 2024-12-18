// Author: Jake Rieger
// Created: 12/16/2024.
//

#pragma once

#include <cstddef>

namespace x::Memory {
    class ArenaAllocator {
    public:
        explicit ArenaAllocator(size_t totalSize);
        ~ArenaAllocator();

        void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));
        void reset();
        [[nodiscard]] size_t getOffset() const;
        [[nodiscard]] size_t getTotalSize() const;

    private:
        void* _memory;
        size_t _totalSize;
        size_t _offset;
        static size_t _align(size_t offset, size_t alignment);
    };
}  // namespace x::Memory