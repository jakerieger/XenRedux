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
        std::vector<Graphics::VertexAttribute> vertexAttributes = {
          {0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0},
          {1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32))},
        };
        _vertexArray = std::make_unique<Graphics::VertexArray>(vertexAttributes, vertices, indices);
        _vertexArray->bind();
        if (!_vertexArray) { Panic("Failed to create vertex array in Mesh instance."); }
    }

    Mesh::~Mesh() {
        _vertexArray.reset();
    }

    void Mesh::update(const std::weak_ptr<Clock>& clock) const {
        static constexpr auto kRotateAmount = 45.0f;
        if (const auto c = clock.lock()) {
            const auto dt = c->getDeltaTime();
            const auto t  = kRotateAmount * dt;
            _transform->rotate({t, t, 0.f});
        } else {
            Panic("Failed to get clock lock in Mesh instance.");
        }
    }

    void Mesh::draw(const std::shared_ptr<ICamera>& camera) const {
        const auto shader = _shader.lock();
        shader->use();
        // TODO: Consider a different way of updating uniforms, because they can be different.
        shader->setMat4("uVP", camera->getViewProjection());
        shader->setMat4("uModel", _transform->getMatrix());
        _vertexArray->bind();
        _vertexArray->bindIndex();
        glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0);
        CHECK_GL_ERROR();
    }

    void Mesh::destroy() {
        _vertexArray.reset();
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