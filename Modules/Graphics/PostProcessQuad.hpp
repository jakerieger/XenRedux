// Author: Jake Rieger
// Created: 12/23/2024.
//

#pragma once

#include "Types.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"

#include <memory>

namespace x::Graphics {
    /// @brief Implements the "big triangle" trick for drawing a fullscreen quad.
    class PostProcessQuad {
    public:
        PostProcessQuad();
        ~PostProcessQuad();

        void draw(u32 renderTexture) const;
        VertexArray* getVertexArray() const;
        std::shared_ptr<ShaderProgram> getShader() const;

    private:
        std::unique_ptr<VertexArray> _vertexArray;
        std::shared_ptr<ShaderProgram> _shader;
    };
}  // namespace x::Graphics