// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "RenderTarget.hpp"
#include "Panic.hpp"

#include <glad.h>

namespace x::Graphics {
    RenderTarget::RenderTarget(const std::shared_ptr<RenderSystem>& renderSystem,
                               int width,
                               int height,
                               const bool depth)
        : _renderSystem(renderSystem) {
        const auto batchQueue = RenderSystem::requestBatchQueue();
        RenderSystem::submitToQueue<Commands::GenFramebufferCommand>(batchQueue, 1, &_fbo);
        RenderSystem::submitToQueue<Commands::BindFramebufferCommand>(batchQueue,
                                                                      GL_FRAMEBUFFER,
                                                                      &_fbo);
        RenderSystem::submitToQueue<Commands::GenTextureCommand>(batchQueue, 1, &_colorTexture);
        RenderSystem::submitToQueue<Commands::BindTextureCommand>(batchQueue,
                                                                  _colorTexture,
                                                                  GL_TEXTURE_2D);
        RenderSystem::submitToQueue<Commands::CreateTexture2DCommand>(batchQueue,
                                                                      GL_TEXTURE_2D,
                                                                      GL_RGBA,
                                                                      width,
                                                                      height,
                                                                      GL_RGBA,
                                                                      GL_UNSIGNED_BYTE);
        RenderSystem::submitToQueue<Commands::TexParameteriCommand>(batchQueue,
                                                                    GL_TEXTURE_2D,
                                                                    GL_TEXTURE_MIN_FILTER,
                                                                    GL_LINEAR);
        RenderSystem::submitToQueue<Commands::TexParameteriCommand>(batchQueue,
                                                                    GL_TEXTURE_2D,
                                                                    GL_TEXTURE_MAG_FILTER,
                                                                    GL_LINEAR);
        RenderSystem::submitToQueue<Commands::CreateFramebufferTexture2DCommand>(
          batchQueue,
          GL_FRAMEBUFFER,
          GL_COLOR_ATTACHMENT0,
          GL_TEXTURE_2D,
          _colorTexture);

        if (depth) {
            RenderSystem::submitToQueue<Commands::GenRenderBufferCommand>(batchQueue,
                                                                          1,
                                                                          &_depthRenderBuffer);
            RenderSystem::submitToQueue<Commands::BindRenderbufferCommand>(batchQueue,
                                                                           _depthRenderBuffer);
            RenderSystem::submitToQueue<Commands::RenderbufferStorageCommand>(batchQueue,
                                                                              GL_RENDERBUFFER,
                                                                              GL_DEPTH_COMPONENT,
                                                                              width,
                                                                              height);
            RenderSystem::submitToQueue<Commands::FramebufferRenderbufferCommand>(
              batchQueue,
              GL_FRAMEBUFFER,
              GL_DEPTH_ATTACHMENT,
              GL_RENDERBUFFER,
              _depthRenderBuffer);
        }

        // TODO: Batch commands above and execute them all here, then check for errors
        RenderSystem::executeQueue(batchQueue);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            // Handle errors
            Panic("Framebuffer is not complete");
        }
        _renderSystem->executeImmediately<Commands::BindFramebufferCommand>(GL_FRAMEBUFFER, 0);
    }

    RenderTarget::~RenderTarget() {
        glDeleteFramebuffers(1, &_fbo);
        glDeleteTextures(1, &_colorTexture);
        glDeleteRenderbuffers(1, &_depthRenderBuffer);
    }

    void RenderTarget::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    }

    void RenderTarget::unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    u32 RenderTarget::getColorTexture() const {
        return _colorTexture;
    }
}  // namespace x::Graphics