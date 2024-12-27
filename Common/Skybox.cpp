// Author: Jake Rieger
// Created: 12/26/2024.
//

#include <glad.h>
#include "Skybox.hpp"
#include "Graphics/Shaders/Include/Skybox_VS.h"
#include "Graphics/Shaders/Include/Skybox_FS.h"

namespace x {
    static constexpr float skyboxVertices[] = {
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
    }

    Skybox::~Skybox() {
        _cubemap.reset();
        _shader.reset();
    }

    void Skybox::update(const std::weak_ptr<Clock>& clock, PerspectiveCamera* camera) {
        _shader->use();
        _shader->setMat4("uVP", camera->getViewProjection());
    }

    void Skybox::draw(PerspectiveCamera* camera) {
        glDepthMask(GL_FALSE);
        _shader->use();
        _cubemap->bind(0);
        _shader->setInt("uSkybox", 0);
        // draw cube vertices
        _cubemap->unbind();
        glDepthMask(GL_TRUE);
    }
}  // namespace x