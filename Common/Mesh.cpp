// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "Mesh.hpp"

namespace x {
    Mesh::Mesh(const std::vector<f32>& vertices,
               const std::vector<u32>& indices,
               const std::weak_ptr<Graphics::ShaderProgram>& shader)
        : _transform(std::make_shared<Transform>()), _shader(shader), _numVertices(vertices.size()),
          _numIndices(indices.size()) {
        glGenVertexArrays(1, &_vao);
        _vertexBuffer = std::make_unique<Memory::GpuBuffer>(Memory::GpuBufferType::Vertex,
                                                            _numVertices * sizeof(f32));
        _vertexBuffer->bind();
        _vertexBuffer->updateData(vertices.data(), 0);
        _indexBuffer = std::make_unique<Memory::GpuBuffer>(Memory::GpuBufferType::Index,
                                                           _numIndices * sizeof(u32));
        _indexBuffer->bind();
        _indexBuffer->updateData(indices.data(), 0);
        glBindVertexArray(_vao);
        _vertexBuffer->bind();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              5 * sizeof(float),
                              (void*)(3 * sizeof(float)));  // Texture coords
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    Mesh::~Mesh() {
        glDeleteVertexArrays(1, &_vao);
        _vertexBuffer.reset();
        _indexBuffer.reset();
    }

    void Mesh::draw(const std::shared_ptr<ICamera>& camera) const {
        const auto shader = _shader.lock();
        shader->use();
        // TODO: Consider a different way of updating uniforms, because they can be different.
        shader->setMat4("uVP", camera->getViewProjection());
        shader->setMat4("uModel", _transform->getMatrix());
        glBindVertexArray(_vao);
        _indexBuffer->bind();
        glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0);
    }

    const Transform& Mesh::getTransform() const {
        return *_transform;
    }

    u32 Mesh::getIndexCount() const {
        return _numIndices;
    }

    u32 Mesh::getVertexCount() const {
        return _numVertices;
    }
}  // namespace x