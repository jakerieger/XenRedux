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
        _renderSystem->executeImmediate<GenBufferCommand>(1, &_id);
        bind(true);
        const auto usage = (data == nullptr) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
        _renderSystem->executeImmediate<BufferDataCommand>(GL_ARRAY_BUFFER, size, data, usage);
    }

    GpuBuffer::~GpuBuffer() {
        _renderSystem->submit<DeleteBufferCommand>(1, &_id);
    }

    void GpuBuffer::bind(bool immediate) const {
        const auto target = (_type == GpuBufferType::Vertex)  ? GL_ARRAY_BUFFER
                            : (_type == GpuBufferType::Index) ? GL_ELEMENT_ARRAY_BUFFER
                                                              : GL_UNIFORM_BUFFER;
        if (immediate) {
            _renderSystem->executeImmediate<BindBufferCommand>(target, _id);
        } else {
            _renderSystem->submit<BindBufferCommand>(target, _id);
        }
    }

    void GpuBuffer::updateData(const void* data, size_t offset) const {
        bind();
        _renderSystem->submit<BufferSubDataCommand>(GL_ARRAY_BUFFER, offset, _size, data);
    }

    u32 GpuBuffer::getId() const {
        return _id;
    }
}  // namespace x::Memory