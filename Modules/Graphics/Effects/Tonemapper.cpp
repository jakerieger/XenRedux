// Author: Jake Rieger
// Created: 12/23/2024.
//

#include "Tonemapper.hpp"
#include "Graphics/Shaders/Include/Tonemapper_CS.h"

namespace x::Graphics {
    TonemapperEffect::TonemapperEffect() {
        const auto computeShader =
          std::make_unique<Shader>(ShaderType::Compute, Tonemapper_CS_Source);
        _shaderProgram = std::make_unique<ShaderProgram>();
        _shaderProgram->attachShader(*computeShader);
        _shaderProgram->link();

        // Create output texture
        glGenTextures(1, &_outputTexture);
        glBindTexture(GL_TEXTURE_2D, _outputTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Create params UBO
        // TODO: Go back and implement this in GpuBuffer
        glGenBuffers(1, &_paramsUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, _paramsUbo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(TonemapperParams), nullptr, GL_DYNAMIC_DRAW);
        updateParams();
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _paramsUbo);
    }

    TonemapperEffect::~TonemapperEffect() {
        glDeleteTextures(1, &_outputTexture);
    }

    void TonemapperEffect::updateParams() const {
        glBindBuffer(GL_UNIFORM_BUFFER, _paramsUbo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TonemapperParams), &_params);
    }

    void TonemapperEffect::apply() const {
        if (getRenderTarget() != 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, getRenderTarget());
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        _shaderProgram->use();
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _paramsUbo);

        glBindImageTexture(1, _outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
        glBindImageTexture(0, getInputTexture(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);

        constexpr u32 kWorkGroups = 8;
        const auto x              = (_textureWidth + (kWorkGroups - 1)) / kWorkGroups;
        const auto y              = (_textureHeight + (kWorkGroups - 1)) / kWorkGroups;
        _shaderProgram->dispatchCompute(x, y, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);  // Ensure compute writes complete
    }

    void TonemapperEffect::setTextureSize(i32 width, i32 height) {
        _textureWidth  = width;
        _textureHeight = height;

        glBindTexture(GL_TEXTURE_2D, _outputTexture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA8,
                     _textureWidth,
                     _textureHeight,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_INT,
                     nullptr);
    }

    u32 TonemapperEffect::getOutputTexture() const {
        return _outputTexture;
    }

    void TonemapperEffect::setGamma(f32 gamma) {
        _params.gamma = gamma;
        glBindBuffer(GL_UNIFORM_BUFFER, _paramsUbo);
        glBufferSubData(GL_UNIFORM_BUFFER,
                        offsetof(TonemapperParams, gamma),
                        sizeof(f32),
                        &_params.gamma);
    }

    void TonemapperEffect::setExposure(f32 exposure) {
        _params.exposure = exposure;
        glBindBuffer(GL_UNIFORM_BUFFER, _paramsUbo);
        glBufferSubData(GL_UNIFORM_BUFFER,
                        offsetof(TonemapperParams, exposure),
                        sizeof(f32),
                        &_params.exposure);
    }

    void TonemapperEffect::setTonemapOperator(i32 op) {
        _params.tonemapOperator = op;
        glBindBuffer(GL_UNIFORM_BUFFER, _paramsUbo);
        glBufferSubData(GL_UNIFORM_BUFFER,
                        offsetof(TonemapperParams, tonemapOperator),
                        sizeof(i32),
                        &_params.tonemapOperator);
    }
}  // namespace x::Graphics