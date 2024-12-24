// Author: Jake Rieger
// Created: 12/23/2024.
//

#include "PostProcessQuad.hpp"
#include "ShaderManager.hpp"

#include <glad.h>

#pragma region Shaders
#include "Shaders/Include/Quad_FS.h"
#include "Shaders/Include/Quad_VS.h"
#pragma endregion

namespace x {
    namespace Graphics {
        PostProcessQuad::PostProcessQuad() {
            _shader = ShaderManager::createProgram(Quad_VS_Source, Quad_FS_Source);
            // clang-format off
            std::vector<f32> vertices = {
                -1.0f, -1.0f,  0.0f, 0.0f,
                 3.0f, -1.0f,  2.0f, 0.0f,
                -1.0f,  3.0f,  0.0f, 2.0f
            };
            // clang-format on
            std::vector<u32> indices                                = {0, 1, 2};
            std::vector<Graphics::VertexAttribute> vertexAttributes = {
              {0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)0},
              {1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)(2 * sizeof(f32))},
            };
            _vertexArray =
              std::make_unique<VertexArray<f32, u32>>(vertexAttributes, vertices, indices);
            _vertexArray->bind();
            if (!_vertexArray) { Panic("Failed to create vertex array in Mesh instance."); }
        }

        PostProcessQuad::~PostProcessQuad() {
            _vertexArray.reset();
        }

        void PostProcessQuad::draw(u32 renderTexture) const {
            _shader->use();
            _shader->setInt("uRenderTexture", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderTexture);
            CHECK_GL_ERROR();

            _vertexArray->bind();
            _vertexArray->bindIndexBuffer();
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
            CHECK_GL_ERROR();
            _vertexArray->unbind();
        }

        VertexArray<f32, u32>* PostProcessQuad::getVertexArray() const {
            return _vertexArray.get();
        }

        std::shared_ptr<ShaderProgram> PostProcessQuad::getShader() const {
            return _shader;
        }
    }  // namespace Graphics
}  // namespace x