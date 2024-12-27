// Author: Jake Rieger
// Created: 12/26/2024.
//

#include <glad.h>
#include "Skybox.hpp"
#include "Graphics/Shaders/Include/Skybox_VS.h"
#include "Graphics/Shaders/Include/Skybox_FS.h"

namespace x {
    static constexpr float kSkyboxVertices[] = {
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

    Skybox::Skybox(const str& filename) {
        _cubemap = std::make_unique<Graphics::Texture>(GL_TEXTURE_CUBE_MAP);
        if (!_cubemap->loadFromFile(filename, true)) { Panic("Failed to load cubemap from file"); }
        _shader = ShaderManager::get().getShaderProgram(Skybox_VS_Source, Skybox_FS_Source);

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(kSkyboxVertices), kSkyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        CHECK_GL_ERROR();
    }

    Skybox::~Skybox() {
        _cubemap.reset();
        _shader.reset();

        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_vbo);
        CHECK_GL_ERROR();
    }

    void Skybox::update(const std::weak_ptr<Clock>& clock, PerspectiveCamera* camera) {}

    void Skybox::draw(PerspectiveCamera* camera) {
        // glDepthMask(GL_FALSE);
        _shader->use();
        _shader->setMat4("uVP", camera->getViewProjection());
        _cubemap->bind(0);
        _shader->setInt("uSkybox", 0);

        // draw cube vertices
        glBindVertexArray(_vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        CHECK_GL_ERROR();

        _cubemap->unbind();
        // glDepthMask(GL_TRUE);
    }
}  // namespace x