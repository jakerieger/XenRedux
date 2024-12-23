// Author: Jake Rieger
// Created: 12/22/2024.
//

#include "VertexArray.hpp"

#include "DebugOpenGL.hpp"

namespace x::Graphics {
    VertexArray::VertexArray(const std::vector<VertexAttribute>& attributes,
                             const std::vector<f32>& vertices,
                             const std::vector<u32>& indices) {
        _attributes   = attributes;
        _vertexBuffer = std::make_unique<Memory::VertexBuffer>(vertices);
        _indexBuffer  = std::make_unique<Memory::IndexBuffer>(indices);
        glGenVertexArrays(1, &_vao);
        CHECK_GL_ERROR();
        if (_vao == 0) { Panic("Vertex Array creation failed"); }
        for (const auto& attribute : _attributes) {
            enableAttribute(attribute);
        }
    }

    VertexArray::~VertexArray() {
        if (!_cleanedUp) cleanup();
    }

    void VertexArray::bind() const {
        glBindVertexArray(_vao);
        CHECK_GL_ERROR();
    }

    void VertexArray::enableAttribute(const VertexAttribute& attr) const {
        bindVertex();
        bind();
        glEnableVertexAttribArray(attr.index);
        glVertexAttribPointer(attr.index,
                              attr.size,
                              attr.type,
                              attr.normalized,
                              attr.stride,
                              attr.offset);
        CHECK_GL_ERROR();
    }

    void VertexArray::disableAttribute(const VertexAttribute& attr) const {
        bindVertex();
        bind();
        glDisableVertexAttribArray(attr.index);
        CHECK_GL_ERROR();
    }

    void VertexArray::cleanup() {
        glDeleteVertexArrays(1, &_vao);
        _cleanedUp = true;
        CHECK_GL_ERROR();
    }

    void VertexArray::unbind() {
        glBindVertexArray(0);
        CHECK_GL_ERROR();
    }

    void VertexArray::unbindBuffers() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        unbind();
        CHECK_GL_ERROR();
    }

    void VertexArray::bindVertex() const {
        _vertexBuffer->bind();
    }

    void VertexArray::bindIndex() const {
        _indexBuffer->bind();
    }
}  // namespace x::Graphics