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
        createRenderTargetCommands(width, height, depth);
    }

    RenderTarget::~RenderTarget() {
        _renderSystem->submitCommand<Commands::DeleteFramebufferCommand>(1, &_fbo)
          ->submitCommand<Commands::DeleteTextureCommand>(1, &_colorTexture)
          ->submitCommand<Commands::DeleteRenderbufferCommand>(1, &_depthRenderBuffer);
    }

    void RenderTarget::bind() const {
        _renderSystem->submitCommand<Commands::BindFramebufferCommand>(GL_FRAMEBUFFER, _fbo);
    }

    void RenderTarget::unbind() const {
        _renderSystem->submitCommand<Commands::BindFramebufferCommand>(GL_FRAMEBUFFER, 0);
    }

    u32 RenderTarget::getColorTexture() const {
        return _colorTexture;
    }

    void RenderTarget::createRenderTargetCommands(int width, int height, bool depth) {
        // Will automatically execute the queue when 'batchQueue' goes out of scope
        ScopedBatchQueue batchQueue;
        batchQueue.submit<Commands::GenFramebufferCommand>(1, &_fbo)
          .submit<Commands::BindFramebufferCommand>(GL_FRAMEBUFFER, &_fbo)
          .submit<Commands::GenTextureCommand>(1, &_colorTexture)
          .submit<Commands::BindTextureCommand>(&_colorTexture)
          .submit<Commands::CreateTexture2DCommand>(GL_TEXTURE_2D,
                                                    GL_RGBA,
                                                    width,
                                                    height,
                                                    GL_RGBA,
                                                    GL_UNSIGNED_BYTE)
          .submit<Commands::TexParameteriCommand>(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
          .submit<Commands::TexParameteriCommand>(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
          .submit<Commands::CreateFramebufferTexture2DCommand>(GL_FRAMEBUFFER,
                                                               GL_COLOR_ATTACHMENT0,
                                                               GL_TEXTURE_2D,
                                                               _colorTexture);

        if (depth) {
            batchQueue.submit<Commands::GenRenderBufferCommand>(1, &_depthRenderBuffer)
              .submit<Commands::BindRenderbufferCommand>(&_depthRenderBuffer)
              .submit<Commands::RenderbufferStorageCommand>(GL_RENDERBUFFER,
                                                            GL_DEPTH_COMPONENT,
                                                            width,
                                                            height)
              .submit<Commands::FramebufferRenderbufferCommand>(GL_FRAMEBUFFER,
                                                                GL_DEPTH_ATTACHMENT,
                                                                GL_RENDERBUFFER,
                                                                _depthRenderBuffer);
        }
        // Unbind the frame buffer when finished
        batchQueue.submit<Commands::BindFramebufferCommand>(GL_FRAMEBUFFER, 0);
    }
}  // namespace x::Graphics