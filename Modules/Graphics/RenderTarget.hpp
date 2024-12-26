// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Types.hpp"
#include "Volatile.hpp"

namespace x::Graphics {
    class RenderTarget final : public Volatile {
    public:
        RenderTarget(int width, int height, bool depth = true);
        ~RenderTarget() override;
        void bind() const;
        void unbind() const;
        [[nodiscard]] u32 getColorTexture() const;
        void onResize(int width, int height) override;  // from Volatile

    private:
        u32 _fbo {};
        u32 _colorTexture {};
        u32 _depthRenderBuffer {};
        bool _hasDepth;
        void createRenderTargetCommands(int width, int height, bool depth);
        void cleanup();
    };
}  // namespace x::Graphics