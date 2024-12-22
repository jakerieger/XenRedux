// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include <glad.h>
#include "Types.hpp"
#include "Panic.hpp"
#include "Transform.hpp"
#include "Graphics/ShaderProgram.hpp"
#include "Memory/GpuBuffer.hpp"
#include "Camera.hpp"

#include <glm/glm.hpp>

namespace x {
    class Mesh {
    public:
        Mesh(const std::vector<f32>& vertices,
             const std::vector<u32>& indices,
             const std::weak_ptr<Graphics::ShaderProgram>& shader);
        ~Mesh();

        void draw(const std::shared_ptr<ICamera>& camera) const;
        const Transform& getTransform() const;
        u32 getIndexCount() const;
        u32 getVertexCount() const;

    private:
        std::shared_ptr<Transform> _transform;
        std::unique_ptr<Memory::GpuBuffer> _vertexBuffer;
        std::unique_ptr<Memory::GpuBuffer> _indexBuffer;
        std::weak_ptr<Graphics::ShaderProgram> _shader;
        const u32 _numVertices;
        const u32 _numIndices;
        GLuint _vao;
    };

}  // namespace x
