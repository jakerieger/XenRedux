// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "RenderSystem.hpp"
#include "Types.hpp"

#include <memory>

namespace x::Graphics {
    class RenderTarget {
    public:
        RenderTarget(const std::shared_ptr<RenderSystem>& renderSystem,
                     int width,
                     int height,
                     bool depth = true);
        ~RenderTarget();
        void bind() const;
        void unbind() const;
        [[nodiscard]] u32 getColorTexture() const;

    private:
        u32 _fbo{};
        u32 _colorTexture{};
        u32 _depthRenderBuffer{};
        std::shared_ptr<RenderSystem> _renderSystem;
        void createRenderTargetCommands(int width, int height, bool depth);
    };
}  // namespace x::Graphics