// Author: Jake Rieger
// Created: 12/23/2024.
//

#pragma once

#include "Graphics/PostProcessEffect.hpp"
#include "Graphics/ShaderProgram.hpp"

#include <memory>

namespace x::Graphics {
    // enum class TonemapperType : i32 {
    //     ACES     = 0,
    //     Reinhard = 1,
    //     Exposure = 2,
    // };

    class TonemapperEffect final : public PostProcessEffect {
    public:
        TonemapperEffect();
        ~TonemapperEffect() override;

        void apply() const override;
        void setTextureSize(i32 width, i32 height);
        u32 getOutputTexture() const;

        void setGamma(f32 gamma);
        void setExposure(f32 exposure);
        /// 0 = ACES, 1 = Reinhard, 2 = Exposure
        void setTonemapOperator(i32 op);

    private:
        std::unique_ptr<ShaderProgram> _shaderProgram;
        u32 _textureWidth  = 0;
        u32 _textureHeight = 0;
        u32 _outputTexture = 0;
        u32 _paramsUbo     = 0;
        struct TonemapperParams {
            f32 exposure        = 1.0f;
            f32 gamma           = 2.2f;
            i32 tonemapOperator = 0;
            i32 padding         = 0;
            // might need padding here depending on your std140 layout
        } _params;

        void updateParams() const;
    };
}  // namespace x::Graphics