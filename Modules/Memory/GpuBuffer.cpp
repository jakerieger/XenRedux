// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "GpuBuffer.hpp"
#include <glad.h>

namespace x::Memory {
    using namespace x::Graphics::Commands;

    GpuBuffer::GpuBuffer(GpuBufferType type, size_t size, const void* data)
        : _type(type), _size(size) {
        glGenBuffers(1, &_id);
        bind();
        glBufferData(getTarget(), size, data, data == nullptr ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
    }

    GpuBuffer::~GpuBuffer() {
        glDeleteBuffers(1, &_id);
    }

    void GpuBuffer::bind() const {
        glBindBuffer(getTarget(), _id);
    }

    void GpuBuffer::updateData(const void* data, size_t offset) const {
        glBufferSubData(getTarget(), offset, _size, data);
    }

    u32 GpuBuffer::getId() const {
        return _id;
    }

    GLenum GpuBuffer::getTarget() const {
        return (_type == GpuBufferType::Vertex)  ? GL_ARRAY_BUFFER
               : (_type == GpuBufferType::Index) ? GL_ELEMENT_ARRAY_BUFFER
                                                 : GL_UNIFORM_BUFFER;
    }
}  // namespace x::Memory