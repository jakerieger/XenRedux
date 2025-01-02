// Author: Jake Rieger
// Created: 12/26/2024.
//

#pragma once

#include "Graphics/PostProcessEffect.hpp"
#include "Graphics/ShaderProgram.hpp"
#include "Graphics/Texture.hpp"

#include <memory>

namespace x::Graphics {
    enum class AATechnique { FXAA, SMAA };

    class AntiAliasing final : public PostProcessEffect {
    public:
        explicit AntiAliasing(const AATechnique technique = AATechnique::FXAA);
        ~AntiAliasing() override;

        void onResize(int newWidth, int newHeight) override;
        void apply() const override;

        void setTechnique(AATechnique tech);
        void setTextureSize(i32 width, i32 height) const;
        u32 getOutputTexture() const;

    private:
        AATechnique _technique;
        std::unique_ptr<ShaderProgram> _shader;
        std::unique_ptr<Texture> _outputTexture;
    };
}  // namespace x::Graphics