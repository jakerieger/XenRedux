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

    namespace Commands {
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

        class TexParameterCommand final : public IRenderCommand {};

        class GenBufferCommand final : public IRenderCommand {
        public:
            GenBufferCommand(int count, u32* id) : _count(count), _id(id) {}

            void execute() override {
                glGenBuffers(_count, _id);
            }

        private:
            int _count;
            u32* _id;
        };

        class GenTextureCommand final : public IRenderCommand {};

        class GenFramebufferCommand final : public IRenderCommand {};

        class GenRenderTargetCommand final : public IRenderCommand {};

        class GenVertexArrayCommand final : public IRenderCommand {};

        class BufferDataCommand final : public IRenderCommand {
        public:
            BufferDataCommand(GLenum target, GLsizei size, const void* data, GLenum usage)
                : _target(target), _usage(usage), _size(size), _data(data) {}

            void execute() override {
                glBufferData(_target, _size, _data, _usage);
            }

        private:
            GLenum _target;
            GLenum _usage;
            GLsizei _size;
            const void* _data;
        };

        class BufferSubDataCommand final : public IRenderCommand {
        public:
            BufferSubDataCommand(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
                : _target(target), _offset(offset), _size(size), _data(data) {}

            void execute() override {
                glBufferSubData(_target, _offset, _size, _data);
            }

        private:
            GLenum _target;
            GLintptr _offset;
            GLsizeiptr _size;
            const void* _data;
        };

        class BindBufferCommand final : public IRenderCommand {
        public:
            BindBufferCommand(GLenum target, GLuint buffer) : _target(target), _id(buffer) {}

            void execute() override {
                glBindBuffer(_target, _id);
            }

        private:
            GLenum _target;
            GLuint _id;
        };

        class BindVertexArrayCommand final : public IRenderCommand {};

        class BindFramebufferCommand final : public IRenderCommand {};

        class BindRenderbufferCommand final : public IRenderCommand {};

        class FramebufferTextureCommand final : public IRenderCommand {};

        class FramebufferRenderbufferCommand final : public IRenderCommand {};

        class DeleteBufferCommand final : public IRenderCommand {
        public:
            DeleteBufferCommand(int count, u32* id) : _count(count), _id(id) {}

            void execute() override {
                glDeleteBuffers(_count, _id);
            }

        private:
            int _count;
            u32* _id;
        };
    }  // namespace Commands
}  // namespace x::Graphics