// Author: Jake Rieger
// Created: 12/18/2024.
//

#include "OpenGLPipelineState.hpp"

namespace x::Graphics {
    OpenGLPipelineState::OpenGLPipelineState(GLuint shaderProgram,
                                             bool depthTest,
                                             GLenum depthFunc,
                                             bool blend,
                                             GLenum blendSrc,
                                             GLenum blendDst,
                                             GLenum cullMode)
        : _shaderProgram(shaderProgram), _depthTest(depthTest), _depthFunc(depthFunc),
          _blend(blend), _blendSrc(blendSrc), _blendDst(blendDst), _cullMode(cullMode) {}

    void OpenGLPipelineState::apply() {
        glUseProgram(_shaderProgram);
        if (_depthTest) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(_depthFunc);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
        if (_blend) {
            glEnable(GL_BLEND);
            glBlendFunc(_blendSrc, _blendDst);
        } else {
            glDisable(GL_BLEND);
        }
        glCullFace(_cullMode);
    }
}  // namespace x::Graphics