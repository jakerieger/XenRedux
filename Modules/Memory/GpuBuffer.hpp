// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Types.hpp"
#include "Context.hpp"
#include "Graphics/DebugOpenGL.hpp"

namespace x::Memory {
    enum GpuBufferType {
        Vertex,
        Index,
        Uniform,
        Texture,
    };

    template<typename T, GpuBufferType Type>
    class GpuBuffer {
    public:
        // Dynamic buffer
        explicit GpuBuffer(const std::vector<T>& data, bool dynamic = false) {
            glGenBuffers(1, &_id);
            _size = data.size() * sizeof(T);
            bind();
            glBufferData(getTarget(),
                         _size,
                         data.data(),
                         dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            CHECK_GL_ERROR();
        }

        ~GpuBuffer() {
            glDeleteBuffers(1, &_id);
            CHECK_GL_ERROR();
        }

        void bind() const {
            glBindBuffer(getTarget(), _id);
            CHECK_GL_ERROR();
        }

        void updateData(const void* data, size_t offset = 0) const {
            glBufferSubData(getTarget(), offset, _size, data);
            CHECK_GL_ERROR();
        }

        u32 getId() const {
            return _id;
        }

    private:
        u32 _id = 0;
        size_t _size;

        GLenum getTarget() const {
            switch (Type) {
                case Vertex:
                    return GL_ARRAY_BUFFER;
                case Index:
                    return GL_ELEMENT_ARRAY_BUFFER;
                case Uniform:
                    return GL_UNIFORM_BUFFER;
                case Texture:
                    return GL_TEXTURE_BUFFER;
            }
            return GL_NONE;
        }
    };
}  // namespace x::Memory