// Author: Jake Rieger
// Created: 12/18/2024.
//

#pragma once

namespace x::Graphics {
    enum class BufferType {
        Vertex,
        Index,
        Uniform,
    };

    enum class BufferUsage {
        StaticDraw,
        DynamicDraw,
        // StreamDraw,
        // StreamDrawIndirect,
        // IndirectDraw,
        // IndirectRead,
        // IndirectWrite
    };

    class IBuffer {
    public:
        virtual ~IBuffer() = default;

        virtual void bind() const                           = 0;
        virtual void unbind() const                         = 0;
        virtual void setData(const void* data, size_t size) = 0;
        virtual size_t getSize() const                      = 0;
        virtual BufferType getType() const                  = 0;
    };
}  // namespace x::Graphics