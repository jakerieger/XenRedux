// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "RenderTarget.hpp"

#include "DebugOpenGL.hpp"
#include "Panic.hpp"

#include <glad.h>

namespace x::Graphics {
    RenderTarget::RenderTarget(int width, int height, const bool depth) {
        createRenderTargetCommands(width, height, depth);
    }

    RenderTarget::~RenderTarget() {
        cleanup();
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

    void RenderTarget::onResize(int width, int height) {
        cleanup();
        createRenderTargetCommands(width, height, _depthRenderBuffer != 0);
    }

    void RenderTarget::createRenderTargetCommands(int width, int height, bool depth) {
        glGenFramebuffers(1, &_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        CHECK_GL_ERROR();

        // Color tex
        glGenTextures(1, &_colorTexture);
        glBindTexture(GL_TEXTURE_2D, _colorTexture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA8,
                     width,
                     height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,
                               _colorTexture,
                               0);
        CHECK_GL_ERROR();

        // Create depth renderbuffer (if requested)
        if (depth) {
            glGenRenderbuffers(1, &_depthRenderBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_DEPTH_ATTACHMENT,
                                      GL_RENDERBUFFER,
                                      _depthRenderBuffer);
            CHECK_GL_ERROR();
        }

        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Panic("Failed to create framebuffer. Status: %x",
                  glCheckFramebufferStatus(GL_FRAMEBUFFER));
        }

        // Unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        CHECK_GL_ERROR();
    }

    void RenderTarget::cleanup() {
        if (_depthRenderBuffer) {
            glDeleteRenderbuffers(1, &_depthRenderBuffer);
            _depthRenderBuffer = 0;
        }
        if (_colorTexture) {
            glDeleteTextures(1, &_colorTexture);
            _colorTexture = 0;
        }
        if (_fbo) {
            glDeleteFramebuffers(1, &_fbo);
            _fbo = 0;
        }
    }
}  // namespace x::Graphics