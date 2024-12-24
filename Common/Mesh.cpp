// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "Mesh.hpp"

namespace x {
    Mesh::Mesh(const std::vector<Graphics::VertexAttribute>& attributes,
               const std::vector<Graphics::VertexPosNormTanBiTanTex>& vertices,
               const std::vector<u32>& indices)
        : _numVertices(CAST<u32>(vertices.size())), _numIndices(CAST<u32>(indices.size())),
          _attributes(attributes) {
        _vertexArray =
          std::make_unique<Graphics::VertexArray<Graphics::VertexPosNormTanBiTanTex, u32>>(
            attributes,
            vertices,
            indices);
        _vertexArray->bind();
        if (!_vertexArray) { Panic("Failed to create vertex array in Mesh instance."); }
    }

    Mesh::~Mesh() {
        _vertexArray.reset();
    }

    void Mesh::update(const std::weak_ptr<Clock>& clock) const {
        // static constexpr auto kRotateAmount = 45.0f;
        // if (const auto c = clock.lock()) {
        //     const auto dt = c->getDeltaTime();
        //     const auto t  = kRotateAmount * dt;
        //     _transform->rotate({t, t, 0.f});
        // } else {
        //     Panic("Failed to get clock lock in Mesh instance.");
        // }
    }

    void Mesh::draw() const {
        _vertexArray->bind();
        _vertexArray->bindIndexBuffer();
        glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0);
        CHECK_GL_ERROR();
    }

    void Mesh::destroy() {
        _vertexArray.reset();
    }

    u32 Mesh::getIndexCount() const {
        return _numIndices;
    }

    u32 Mesh::getVertexCount() const {
        return _numVertices;
    }
}  // namespace x