// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Types.hpp"

namespace x::Graphics {
    class RenderTarget {
    public:
        RenderTarget(int width, int height, bool depth = true);
        ~RenderTarget();
        void bind() const;
        void unbind() const;
        u32 getColorTexture() const;

    private:
        u32 _fbo;
        u32 _colorTexture;
        u32 _depthRenderBuffer;
    };
}  // namespace x::Graphics