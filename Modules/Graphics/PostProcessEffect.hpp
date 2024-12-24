// Author: Jake Rieger
// Created: 12/23/2024.
//

#pragma once

#include "Types.hpp"

namespace x::Graphics {
    class IPostProcessEffect {
    public:
        virtual ~IPostProcessEffect()                       = default;
        virtual void setInputTexture(const u32 texture)     = 0;
        virtual void setRenderTarget(const u32 frameBuffer) = 0;
        virtual void apply() const                          = 0;
    };

    class PostProcessEffect : public IPostProcessEffect {
    public:
        PostProcessEffect() : _inputTexture(0), _renderTarget(0) {}

        void setInputTexture(const u32 texture) override {
            _inputTexture = texture;
        }

        void setRenderTarget(const u32 frameBuffer) override {
            _renderTarget = frameBuffer;
        }

        u32 getInputTexture() const {
            return _inputTexture;
        }

        u32 getRenderTarget() const {
            return _renderTarget;
        }

    private:
        u32 _inputTexture;
        u32 _renderTarget;
    };
}  // namespace x::Graphics