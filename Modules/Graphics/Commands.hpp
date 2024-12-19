// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Types.hpp"
#include <glad.h>

namespace x::Graphics {
    class IRenderCommand {
    public:
        virtual ~IRenderCommand() = default;
        virtual void execute()    = 0;
    };

    class ClearCommand final : public IRenderCommand {
    public:
        ClearCommand(float r, float g, float b, float a) : _r(r), _g(g), _b(b), _a(a) {}

        void execute() override {
            glClearColor(_r, _g, _b, _a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

    private:
        float _r, _g, _b, _a;
    };

    class BindTextureCommand final : public IRenderCommand {
    public:
        explicit BindTextureCommand(u32 textureId, GLenum target = GL_TEXTURE_2D)
            : _textureId(textureId), _target(target) {}

        void execute() override {
            glBindTexture(_target, _textureId);
        }

    private:
        u32 _textureId;
        GLenum _target;
    };

    class DrawElementsCommand final : public IRenderCommand {
    public:
        DrawElementsCommand(GLenum mode, GLsizei count, GLenum type, const void* indices)
            : _mode(mode), _count(count), _type(type), _indices(indices) {}

        void execute() override {
            glDrawElements(_mode, _count, _type, _indices);
        }

    private:
        GLenum _mode;
        GLsizei _count;
        GLenum _type;
        const void* _indices;
    };

    class DrawArraysCommand : public IRenderCommand {};
}  // namespace x::Graphics