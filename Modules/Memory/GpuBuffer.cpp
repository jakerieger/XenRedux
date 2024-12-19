// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "GpuBuffer.hpp"
#include <glad.h>

namespace x::Memory {
    using namespace x::Graphics::Commands;

    GpuBuffer::GpuBuffer(const std::shared_ptr<RenderSystem>& renderSystem,
                         GpuBufferType type,
                         size_t size,
                         const void* data)
        : _type(type), _size(size), _renderSystem(renderSystem) {
        _renderSystem->executeImmediately<GenBufferCommand>(1, &_id);
        bind();
        const auto usage = (data == nullptr) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
        _renderSystem->executeImmediately<BufferDataCommand>(GL_ARRAY_BUFFER, size, data, usage);
    }

    GpuBuffer::~GpuBuffer() {
        _renderSystem->submitCommand<DeleteBufferCommand>(1, &_id);
    }

    void GpuBuffer::bind() const {
        const auto target = (_type == GpuBufferType::Vertex)  ? GL_ARRAY_BUFFER
                            : (_type == GpuBufferType::Index) ? GL_ELEMENT_ARRAY_BUFFER
                                                              : GL_UNIFORM_BUFFER;
        _renderSystem->submitCommand<BindBufferCommand>(target, _id);
    }

    void GpuBuffer::updateData(const void* data, size_t offset) const {
        bind();
        _renderSystem->submitCommand<BufferSubDataCommand>(GL_ARRAY_BUFFER, offset, _size, data);
    }

    u32 GpuBuffer::getId() const {
        return _id;
    }
}  // namespace x::Memory