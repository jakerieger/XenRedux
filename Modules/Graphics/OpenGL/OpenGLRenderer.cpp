// Author: Jake Rieger
// Created: 12/18/2024.
//

#include "OpenGlRenderer.hpp"
#include "Panic.hpp"
#include <glad.h>

namespace x::Graphics {
    OpenGLRenderer::OpenGLRenderer(void* window) : _window(window) {}

    OpenGLRenderer::~OpenGLRenderer() {}

    void OpenGLRenderer::initialize() {}

    void OpenGLRenderer::setViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
    }

    void OpenGLRenderer::setScissor(int x, int y, int width, int height) {
        glScissor(x, y, width, height);
    }

    void OpenGLRenderer::setPipeline(IPipelineState* state) {
        auto* pipelineState = DCAST<IPipelineState*>(state);
        if (!pipelineState) { Panic("Invalid pipeline state type passed to OpenGLRenderer"); }
        pipelineState->apply();
    }

    void OpenGLRenderer::submitDrawCall(IBuffer* vertexBuffer, IBuffer* indexBuffer) {
        vertexBuffer->bind();
        indexBuffer->bind();
        glDrawElements(GL_TRIANGLES, indexBuffer->getSize(), GL_UNSIGNED_INT, 0);
        vertexBuffer->unbind();
        indexBuffer->unbind();
    }

    void OpenGLRenderer::clear(f32 r, f32 g, f32 b, f32 a) {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::present() {
        // Swap buffers - platform dependent
    }
}  // namespace x::Graphics