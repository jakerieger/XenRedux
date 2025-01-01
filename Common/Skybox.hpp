// Author: Jake Rieger
// Created: 12/26/2024.
//

#pragma once

#include "Camera.hpp"
#include "Clock.hpp"
#include "ShaderManager.hpp"
#include "Types.hpp"
#include "Graphics/ShaderProgram.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/VertexArray.hpp"

namespace x {
    class Skybox : public Volatile {
    public:
        explicit Skybox(const str& filename);
        ~Skybox() override;

        void setCubemap(Graphics::Texture& cubemap) {
            _cubemap.reset(&cubemap);
        }
        void update(const std::weak_ptr<Clock>& clock, const std::shared_ptr<ICamera>& camera);
        void draw(i32 cubemapId = -1);
        void onResize(int newWidth, int newHeight) override;

    private:
        std::unique_ptr<Graphics::Texture> _cubemap;
        std::shared_ptr<Graphics::ShaderProgram> _shader;
        GLuint _vbo;
        GLuint _vao;
    };
}  // namespace x
