// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "RenderTarget.hpp"
#include "Panic.hpp"

#include <glad.h>

namespace x::Graphics {
    RenderTarget::RenderTarget(int width, int height, bool depth) {
        glGenFramebuffers(1, &_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glGenTextures(1, &_colorTexture);
        glBindTexture(GL_TEXTURE_2D, _colorTexture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
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
        if (depth) {
            glGenRenderbuffers(1, &_depthRenderBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_DEPTH_ATTACHMENT,
                                      GL_RENDERBUFFER,
                                      _depthRenderBuffer);
        }
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            // Handle errors
            Panic("Framebuffer is not complete");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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