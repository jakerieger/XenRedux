// Author: Jake Rieger
// Created: 12/22/2024.
//

#include "VertexArray.hpp"

namespace x {
    namespace Graphics {
        VertexArray::VertexArray(const std::vector<VertexAttribute>& attributes,
                                 const std::vector<f32>& vertices,
                                 const std::vector<u32>& indices) {
            _attributes   = attributes;
            _vertexBuffer = std::make_unique<Memory::VertexBuffer>(vertices);
            _indexBuffer  = std::make_unique<Memory::IndexBuffer>(indices);
            glGenVertexArrays(1, &_vao);
            if (_vao == 0) { Panic("Vertex Array creation failed"); }
            for (const auto& attribute : _attributes) {
                enableAttribute(attribute);
            }
        }

        VertexArray::~VertexArray() {
            glDeleteVertexArrays(1, &_vao);
        }

        void VertexArray::bind() const {
            glBindVertexArray(_vao);
        }

        void VertexArray::enableAttribute(const VertexAttribute& attr) const {
            bindVertex();
            bind();
            glEnableVertexAttribArray(attr.index);
            glVertexAttribPointer(attr.index,
                                  attr.size,
                                  attr.type,
                                  attr.normalized,
                                  attr.size,
                                  attr.offset);
        }

        void VertexArray::disableAttribute(const VertexAttribute& attr) const {
            bindVertex();
            bind();
            glDisableVertexAttribArray(attr.index);
        }

        void VertexArray::unbind() {
            glBindVertexArray(0);
        }

        void VertexArray::unbindBuffers() {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            unbind();
        }

        void VertexArray::bindVertex() const {
            _vertexBuffer->bind();
        }

        void VertexArray::bindIndex() const {
            _indexBuffer->bind();
        }
    }  // namespace Graphics
}  // namespace x