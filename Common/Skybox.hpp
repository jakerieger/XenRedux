// Author: Jake Rieger
// Created: 12/26/2024.
//

#pragma once

#include "Camera.hpp"
#include "PerspectiveCamera.hpp"
#include "ShaderManager.hpp"
#include "Types.hpp"
#include "Graphics/ShaderProgram.hpp"
#include "Graphics/Texture.hpp"

#include "Graphics/Shaders/Include/Skybox_VS.h"
#include "Graphics/Shaders/Include/Skybox_FS.h"

namespace x {
    class Skybox {
    public:
        explicit Skybox(const str& filename) {
            _cubemap = std::make_unique<Graphics::Texture>(GL_TEXTURE_CUBE_MAP);
            if (!_cubemap->loadFromFile(filename, true)) {
                Panic("Failed to load cubemap from file");
            }
            _shader = ShaderManager::get().getShaderProgram(Skybox_VS_Source, Skybox_FS_Source);
        }

        ~Skybox() {
            _cubemap.reset();
        }

        void update(const std::weak_ptr<Clock>& clock) {}

        void draw(PerspectiveCamera* camera) {
            _cubemap->bind(0);
            _cubemap->unbind();
        }

    private:
        std::unique_ptr<Graphics::Texture> _cubemap;
        std::shared_ptr<Graphics::ShaderProgram> _shader;
    };
}  // namespace x
