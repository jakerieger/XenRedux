// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

// Type conversion warnings
#pragma warning(disable : 4244 4267)

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

        class ViewportCommand final : public IRenderCommand {
        public:
            ViewportCommand(int x, int y, int width, int height)
                : _width(width), _height(height), _x(x), _y(y) {}

            void execute() override {
                glViewport(_x, _y, _width, _height);
            }

        private:
            float _width, _height;
            float _x, _y;
        };

        class BindTextureCommand final : public IRenderCommand {
        public:
            explicit BindTextureCommand(const u32 textureId, const GLenum target = GL_TEXTURE_2D)
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

        class TexParameteriCommand final : public IRenderCommand {
        public:
            TexParameteriCommand(const GLenum target, const GLenum name, const GLenum param)
                : _target(target), _name(name), _param(param) {}

            void execute() override {
                glTexParameteri(_target, _name, _param);
            }

        private:
            GLenum _target;
            GLenum _name;
            GLenum _param;
        };

        class CreateTexture2DCommand final : public IRenderCommand {
        public:
            CreateTexture2DCommand(
              GLenum target, GLenum internalFmt, u32 width, u32 height, GLenum format, GLenum type)
                : _target(target), _internalFormat(internalFmt), _width(width), _height(height),
                  _format(format), _type(type) {}

            void execute() override {
                glTexImage2D(_target,
                             0,
                             _internalFormat,
                             _width,
                             _height,
                             0,
                             _format,
                             _type,
                             nullptr);
            }

        private:
            GLenum _target;
            GLenum _internalFormat;
            GLenum _format;
            GLenum _type;
            u32 _width, _height;
        };

        class CreateFramebufferTexture2DCommand final : public IRenderCommand {
        public:
            CreateFramebufferTexture2DCommand(const GLenum target,
                                              const GLenum attachment,
                                              const GLenum texTarget,
                                              const u32 id)
                : _target(target), _attachment(attachment), _texTarget(texTarget), _id(id) {}

            void execute() override {
                glFramebufferTexture2D(_target, _attachment, _texTarget, _id, 0);
            }

        private:
            GLenum _target;
            GLenum _attachment;
            GLenum _texTarget;
            u32 _id;
        };

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

        class GenTextureCommand final : public IRenderCommand {
        public:
            GenTextureCommand(const GLsizei count, u32* id) : _count(count), _id(id) {}

            void execute() override {
                glGenTextures(_count, _id);
            }

        private:
            GLsizei _count;
            u32* _id;
        };

        class GenFramebufferCommand final : public IRenderCommand {
        public:
            GenFramebufferCommand(const GLsizei count, u32* id) : _count(count), _id(id) {}

            void execute() override {
                glGenFramebuffers(_count, _id);
            }

        private:
            GLsizei _count;
            GLuint* _id;
        };

        class GenRenderBufferCommand final : public IRenderCommand {
        public:
            GenRenderBufferCommand(const GLsizei count, u32* id) : _count(count), _id(id) {}

            void execute() override {
                glGenRenderbuffers(_count, _id);
            }

        private:
            GLsizei _count;
            u32* _id;
        };

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

        class BindFramebufferCommand final : public IRenderCommand {
        public:
            BindFramebufferCommand(GLenum target, GLuint buffer)
                : _target(target), _buffer(buffer) {}

            void execute() override {
                glBindFramebuffer(_target, _buffer);
            }

        private:
            GLenum _target;
            GLuint _buffer;
        };

        class BindRenderbufferCommand final : public IRenderCommand {
        public:
            BindRenderbufferCommand(const GLenum target, const u32 buffer)
                : _target(target), _id(buffer) {}

            void execute() override {
                glBindRenderbuffer(_target, _id);
            }

        private:
            GLenum _target;
            u32 _id;
        };

        class RenderbufferStorageCommand final : public IRenderCommand {
        public:
            RenderbufferStorageCommand(const GLenum target,
                                       const GLenum internalformat,
                                       GLsizei width,
                                       GLsizei height)
                : _target(target), _internalFormat(internalformat), _width(width), _height(height) {
            }

            void execute() override {
                glRenderbufferStorage(_target, _internalFormat, _width, _height);
            }

        private:
            GLenum _target;
            GLenum _internalFormat;
            GLsizei _width, _height;
        };

        class FramebufferRenderbufferCommand final : public IRenderCommand {
        public:
            FramebufferRenderbufferCommand(const GLenum target,
                                           const GLenum internalformat,
                                           const GLenum renderbuffer,
                                           u32 id)
                : _target(target), _internalFormat(internalformat), _rbTarget(renderbuffer),
                  _rbId(id) {}

            void execute() override {
                glFramebufferRenderbuffer(_target, _internalFormat, _rbTarget, _rbId);
            }

        private:
            GLenum _target;
            GLenum _internalFormat;
            GLenum _rbTarget;
            u32 _rbId;
        };

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

        class DeleteFramebufferCommand final : public IRenderCommand {
        public:
            DeleteFramebufferCommand(int count, u32* id) : _count(count), _id(id) {}

            void execute() override {
                glDeleteFramebuffers(_count, _id);
            }

        private:
            GLsizei _count;
            GLuint* _id;
        };

        class DeleteRenderbufferCommand final : public IRenderCommand {
        public:
            DeleteRenderbufferCommand(int count, u32* id) : _count(count), _id(id) {}

            void execute() override {
                glDeleteRenderbuffers(_count, _id);
            }

        private:
            GLsizei _count;
            GLuint* _id;
        };

        class DeleteTextureCommand final : public IRenderCommand {
        public:
            DeleteTextureCommand(int count, u32* id) : _count(count), _id(id) {}

            void execute() override {
                glDeleteTextures(_count, _id);
            }

        private:
            GLsizei _count;
            GLuint* _id;
        };

        class DeleteVertexArrayCommand final : public IRenderCommand {};

        class BindVertexArrayCommand final : public IRenderCommand {};
    }  // namespace Commands
}  // namespace x::Graphics