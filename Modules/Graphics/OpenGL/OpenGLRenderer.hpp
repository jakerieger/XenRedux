// Author: Jake Rieger
// Created: 12/18/2024.
//

#pragma once

#include "Graphics/Renderer.hpp"

namespace x::Graphics {
    class OpenGLRenderer final : public IRenderer {
    public:
        OpenGLRenderer(void* window);
        ~OpenGLRenderer() override;

        void initialize() override;
        void setViewport(int x, int y, int width, int height) override;
        void setScissor(int x, int y, int width, int height) override;
        void setPipeline(IPipelineState* state) override;
        void submitDrawCall(IBuffer* vertexBuffer, IBuffer* indexBuffer) override;
        void clear(f32 r, f32 g, f32 b, f32 a) override;
        void present() override;

    private:
        void* _window;
    };
}  // namespace x::Graphics