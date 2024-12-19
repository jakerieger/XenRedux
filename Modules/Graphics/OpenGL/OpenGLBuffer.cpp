// Author: Jake Rieger
// Created: 12/18/2024.
//

#include "OpenGLBuffer.hpp"

namespace x::Graphics {
    OpenGLBuffer::OpenGLBuffer(BufferType type, BufferUsage usage)
        : _type(type), _usage(usage), _size(0) {
        glGenBuffers(1, &_bufferId);
        switch (type) {
            case BufferType::Vertex:
                _oglType = GL_ARRAY_BUFFER;
                break;
            case BufferType::Index:
                _oglType = GL_ELEMENT_ARRAY_BUFFER;
                break;
            case BufferType::Uniform:
                _oglType = GL_UNIFORM_BUFFER;
                break;
        }
        switch (usage) {
            case BufferUsage::StaticDraw:
                _oglUsage = GL_STATIC_DRAW;
                break;
            case BufferUsage::DynamicDraw:
                _oglUsage = GL_DYNAMIC_DRAW;
                break;
        }
    }

    OpenGLBuffer::~OpenGLBuffer() {
        glDeleteBuffers(1, &_bufferId);
    }

    void OpenGLBuffer::bind() const {
        glBindBuffer(_oglType, _bufferId);
    }

    void OpenGLBuffer::unbind() const {
        glBindBuffer(_oglType, 0);
    }

    void OpenGLBuffer::setData(const void* data, size_t size) {
        bind();
        glBufferData(_oglType, size, data, _oglUsage);
        _size = size;
    }

    size_t OpenGLBuffer::getSize() const {
        return _size;
    }

    BufferType OpenGLBuffer::getType() const {
        return _type;
    }
}  // namespace x::Graphics