// Author: Jake Rieger
// Created: 12/23/2024.
//

#include "GaussianBlur.hpp"
#include "Graphics/Shaders/Include/Gaussian_CS.h"

namespace x::Graphics {
    static constexpr size_t kKernelSize         = 9;
    static constexpr float kKernel[kKernelSize] = {
      0.0545f, 0.0850f, 0.1185f, 0.1475f, 0.1600f, 0.1475f, 0.1185f, 0.0850f, 0.0545f};

    GaussianBlurEffect::GaussianBlurEffect() {
        const auto computeShader =
          std::make_unique<Shader>(ShaderType::Compute, Gaussian_CS_Source);
        _shaderProgram = std::make_unique<ShaderProgram>();
        _shaderProgram->attachShader(*computeShader);
        _shaderProgram->link();

        // Create output texture
        glGenTextures(1, &_outputTexture);
        glBindTexture(GL_TEXTURE_2D, _outputTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1, 1, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    GaussianBlurEffect::~GaussianBlurEffect() {
        glDeleteTextures(1, &_outputTexture);
    }

    void GaussianBlurEffect::apply() const {
        if (getRenderTarget() != 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, getRenderTarget());
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getInputTexture());
        glBindImageTexture(0, _outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        _shaderProgram->use();
        _shaderProgram->setInt("uInputTexture", 0);
        _shaderProgram->setVec2i("uTextureSize", _textureWidth, _textureHeight);
        _shaderProgram->setFloatArray("uKernel", kKernel, kKernelSize);
        _shaderProgram->setFloat("uBlurStrength", _blurStrength);

        constexpr u32 kWorkGroups = 16;
        const auto x              = (_textureWidth + (kWorkGroups - 1)) / kWorkGroups;
        const auto y              = (_textureHeight + (kWorkGroups - 1)) / kWorkGroups;
        _shaderProgram->dispatchCompute(x, y, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);  // Ensure compute writes complete
    }

    void GaussianBlurEffect::setTextureSize(i32 width, i32 height) {
        _textureWidth  = width;
        _textureHeight = height;

        glBindTexture(GL_TEXTURE_2D, _outputTexture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA32F,
                     _textureWidth,
                     _textureHeight,
                     0,
                     GL_RGBA,
                     GL_FLOAT,
                     nullptr);
    }

    void GaussianBlurEffect::setBlurStrength(f32 strength) {
        _blurStrength = strength;
    }

    u32 GaussianBlurEffect::getOutputTexture() const {
        return _outputTexture;
    }
}  // namespace x::Graphics