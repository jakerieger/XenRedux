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
        GpuBuffer(const std::shared_ptr<RenderSystem>& renderSystem,
                  GpuBufferType type,
                  size_t size,
                  const void* data = nullptr);
        ~GpuBuffer();
        void bind(bool immediate = false) const;
        void updateData(const void* data, size_t offset = 0) const;
        u32 getId() const;

    private:
        u32 _id{};
        GpuBufferType _type;
        size_t _size;
        std::shared_ptr<RenderSystem> _renderSystem;
    };
}  // namespace x::Memory