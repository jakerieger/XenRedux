// Author: Jake Rieger
// Created: 12/23/2024.
//

#pragma once

#include "Graphics/PostProcessEffect.hpp"
#include "Graphics/ShaderProgram.hpp"

#include <memory>

namespace x::Graphics {
    class GaussianBlurEffect final : public PostProcessEffect {
    public:
        GaussianBlurEffect();
        ~GaussianBlurEffect() override;

        void apply() const override;
        void setTextureSize(i32 width, i32 height);
        void setBlurStrength(f32 strength);
        u32 getOutputTexture() const;

    private:
        std::unique_ptr<ShaderProgram> _shaderProgram;
        u32 _textureWidth  = 0;
        u32 _textureHeight = 0;
        u32 _outputTexture = 0;
        f32 _blurStrength  = 0.5f;
    };
}  // namespace x::Graphics