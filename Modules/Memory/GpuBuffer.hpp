// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Types.hpp"
#include "RenderSystem.hpp"

namespace x::Memory {
    enum class GpuBufferType {
        Vertex,
        Index,
        Uniform,
        Texture,
    };

    class GpuBuffer {
    public:
        GpuBuffer(GpuBufferType type, size_t size, const void* data = nullptr);
        ~GpuBuffer();
        void bind() const;
        void updateData(const void* data, size_t offset = 0) const;
        u32 getId() const;

    private:
        u32 _id = 0;
        GpuBufferType _type;
        size_t _size;
        GLenum getTarget() const;
    };
}  // namespace x::Memory