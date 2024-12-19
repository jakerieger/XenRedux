// Author: Jake Rieger
// Created: 12/18/2024.
//

#pragma once

#include "Graphics/Buffer.hpp"
#include <glad.h>

namespace x::Graphics {
    class OpenGLBuffer final : public IBuffer {
    public:
        OpenGLBuffer(BufferType type, BufferUsage usage);
        ~OpenGLBuffer() override;
        void bind() const override;
        void unbind() const override;
        void setData(const void* data, size_t size) override;
        size_t getSize() const override;
        BufferType getType() const override;

    private:
        GLuint _bufferId;
        GLenum _oglType;
        GLenum _oglUsage;
        BufferType _type;
        BufferUsage _usage;
        size_t _size;
    };
}  // namespace x::Graphics