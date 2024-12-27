// Author: Jake Rieger
// Created: 12/26/2024.
//

#include "Panic.hpp"
#include "AntiAliasing.hpp"
#include "Graphics/Shaders/Include/FXAA_CS.h"

namespace x::Graphics {
    AntiAliasing::AntiAliasing(const AATechnique technique) : _technique(technique) {
        const auto computeShader = std::make_unique<Shader>(ShaderType::Compute, FXAA_CS_Source);
        _shader                  = std::make_unique<ShaderProgram>();
        _shader->attachShader(*computeShader);
        _shader->link();

        _outputTexture = std::make_unique<Texture2D>();
        if (!_outputTexture->create(0, 0, GL_RGBA16F)) { Panic("Failed to create output texture"); }
        _outputTexture->setFilterMode(GL_LINEAR, GL_LINEAR);
        _outputTexture->setWrapMode(GL_CLAMP_TO_EDGE);
    }

    AntiAliasing::~AntiAliasing() {
        _outputTexture.reset();
        _shader.reset();
    }

    void AntiAliasing::onResize(int newWidth, int newHeight) {
        setTextureSize(newWidth, newHeight);
    }

    void AntiAliasing::apply() const {
        if (getRenderTarget() != 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, getRenderTarget());
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        _shader->use();
        _outputTexture->bindImage(0, GL_WRITE_ONLY, GL_RGBA16F);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getInputTexture());
        _shader->setInt("uInputTexture", 0);

        constexpr u32 kWorkGroups = 16;
        const auto x              = (_outputTexture->getWidth() + (kWorkGroups - 1)) / kWorkGroups;
        const auto y              = (_outputTexture->getHeight() + (kWorkGroups - 1)) / kWorkGroups;
        _shader->dispatchCompute(x, y, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);  // Ensure compute writes complete
    }

    void AntiAliasing::setTechnique(AATechnique tech) {
        _technique = tech;
    }

    void AntiAliasing::setTextureSize(i32 width, i32 height) const {
        _outputTexture->resize(width, height);
    }

    u32 AntiAliasing::getOutputTexture() const {
        return _outputTexture->getId();
    }
}  // namespace x::Graphics