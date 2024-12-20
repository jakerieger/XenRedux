// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "RenderSystem.hpp"
#include "Types.hpp"
#include "Volatile.hpp"

#include <memory>

namespace x::Graphics {
    class RenderTarget final : public Volatile {
    public:
        RenderTarget(const std::shared_ptr<RenderSystem>& renderSystem,
                     int width,
                     int height,
                     bool depth = true);
        ~RenderTarget() override;
        void bind() const;
        void unbind() const;
        [[nodiscard]] u32 getColorTexture() const;
        void onResize(int width, int height) override;  // from Volatile

    private:
        u32 _fbo {};
        u32 _colorTexture {};
        u32 _depthRenderBuffer {};
        std::shared_ptr<RenderSystem> _renderSystem;
        void createRenderTargetCommands(int width, int height, bool depth);
    };
}  // namespace x::Graphics