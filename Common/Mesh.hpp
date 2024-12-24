// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include <glad.h>
#include "Types.hpp"
#include "Clock.hpp"
#include "Graphics/VertexArray.hpp"

namespace x {
    class Mesh {
    public:
        Mesh(const std::vector<Graphics::VertexAttribute>& attributes,
             const std::vector<Graphics::VertexPosNormTanBiTanTex>& vertices,
             const std::vector<u32>& indices);
        ~Mesh();

        void update(const std::weak_ptr<Clock>& clock) const;
        void draw() const;
        void destroy();

        u32 getIndexCount() const;
        u32 getVertexCount() const;

    private:
        std::unique_ptr<Graphics::VertexArray<Graphics::VertexPosNormTanBiTanTex, u32>>
          _vertexArray;
        const u32 _numVertices;
        const u32 _numIndices;
        const std::vector<Graphics::VertexAttribute>& _attributes;
    };

}  // namespace x
