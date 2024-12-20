// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "RenderTarget.hpp"
#include "Panic.hpp"
#include "ScopedBatchQueue.hpp"

#include <glad.h>

namespace x::Graphics {
    RenderTarget::RenderTarget(const std::shared_ptr<RenderSystem>& renderSystem,
                               int width,
                               int height,
                               const bool depth)
        : _renderSystem(renderSystem) {
        if (!_renderSystem) { Panic("RenderSystem is null!"); }
        createRenderTargetCommands(width, height, depth);
        _renderSystem->registerVolatile(this);
    }

    RenderTarget::~RenderTarget() {
        if (!_renderSystem) { Panic("RenderSystem is null!"); }
        _renderSystem->submit<Commands::DeleteFramebufferCommand>(1, &_fbo)
          ->submit<Commands::DeleteTextureCommand>(1, &_colorTexture)
          ->submit<Commands::DeleteRenderbufferCommand>(1, &_depthRenderBuffer);
    }

    void RenderTarget::bind() const {
        if (!_renderSystem) { Panic("RenderSystem is null!"); }
        _renderSystem->submit<Commands::BindFramebufferCommand>(GL_FRAMEBUFFER, _fbo);
    }

    void RenderTarget::unbind() const {
        if (!_renderSystem) { Panic("RenderSystem is null!"); }
        _renderSystem->submit<Commands::BindFramebufferCommand>(GL_FRAMEBUFFER, 0);
    }

    u32 RenderTarget::getColorTexture() const {
        return _colorTexture;
    }

    void RenderTarget::onResize(int width, int height) {
        // Ensure valid dimensions
        if (width <= 0 || height <= 0) { Panic("Invalid width/height (< zero)!"); }
        if (!_renderSystem) { Panic("RenderSystem is null!"); }

        _renderSystem->executeImmediate<Commands::DeleteFramebufferCommand>(1, &_fbo)
          ->executeImmediate<Commands::DeleteTextureCommand>(1, &_colorTexture);
        if (_depthRenderBuffer) {
            _renderSystem->executeImmediate<Commands::DeleteRenderbufferCommand>(
              1,
              &_depthRenderBuffer);
        }

        createRenderTargetCommands(width, height, _depthRenderBuffer != 0);
    }

    // TODO: Batch queue appears to be broken, re-vist the `submit` method
    void RenderTarget::createRenderTargetCommands(int width, int height, bool depth) {
        // Will automatically execute the queue when 'batchQueue' goes out of scope
        ScopedBatchQueue batchQueue;
        batchQueue.submit<Commands::GenFramebufferCommand>(1, &_fbo)
          ->submit<Commands::BindFramebufferCommand>(GL_FRAMEBUFFER, _fbo)
          ->submit<Commands::GenTextureCommand>(1, &_colorTexture)
          ->submit<Commands::BindTextureCommand>(_colorTexture)
          ->submit<Commands::CreateTexture2DCommand>(GL_TEXTURE_2D,
                                                     GL_RGBA,
                                                     width,
                                                     height,
                                                     GL_RGBA,
                                                     GL_UNSIGNED_BYTE)
          ->submit<Commands::TexParameteriCommand>(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
          ->submit<Commands::TexParameteriCommand>(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
          ->submit<Commands::CreateFramebufferTexture2DCommand>(GL_FRAMEBUFFER,
                                                                GL_COLOR_ATTACHMENT0,
                                                                GL_TEXTURE_2D,
                                                                _colorTexture);

        if (depth) {
            batchQueue.submit<Commands::GenRenderBufferCommand>(1, &_depthRenderBuffer)
              ->submit<Commands::BindRenderbufferCommand>(GL_RENDERBUFFER, _depthRenderBuffer)
              ->submit<Commands::RenderbufferStorageCommand>(GL_RENDERBUFFER,
                                                             GL_DEPTH_COMPONENT,
                                                             width,
                                                             height)
              ->submit<Commands::FramebufferRenderbufferCommand>(GL_FRAMEBUFFER,
                                                                 GL_DEPTH_ATTACHMENT,
                                                                 GL_RENDERBUFFER,
                                                                 _depthRenderBuffer);
        }
        // Unbind the frame buffer when finished
        batchQueue.submit<Commands::BindFramebufferCommand>(GL_FRAMEBUFFER, 0);
    }
}  // namespace x::Graphics