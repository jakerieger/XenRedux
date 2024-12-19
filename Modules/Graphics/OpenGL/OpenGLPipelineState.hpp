// Author: Jake Rieger
// Created: 12/18/2024.
//

#pragma once

#include "Graphics/Renderer.hpp"
#include <glad.h>

namespace x::Graphics {
    class OpenGLPipelineState final : public IPipelineState {
    public:
        OpenGLPipelineState(GLuint shaderProgram,
                            bool depthTest,
                            GLenum depthFunc,
                            bool blend,
                            GLenum blendSrc,
                            GLenum blendDst,
                            GLenum cullMode);
        void apply() override;

    private:
        GLuint _shaderProgram;
        bool _depthTest;
        GLenum _depthFunc;
        bool _blend;
        GLenum _blendSrc;
        GLenum _blendDst;
        GLenum _cullMode;
    };
}  // namespace x::Graphics