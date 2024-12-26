// Author: Jake Rieger
// Created: 12/23/2024.
//

#pragma once

#include <glad.h>

#include "Panic.hpp"
#include "Types.hpp"
#include "Data/BinaryData.hpp"

namespace x::Graphics {
    class Texture2D {
    public:
        Texture2D() = default;
        ~Texture2D();

        Texture2D(const Texture2D&)            = delete;
        Texture2D& operator=(const Texture2D&) = delete;
        Texture2D(Texture2D&&)                 = delete;
        Texture2D& operator=(Texture2D&&)      = delete;

        bool loadFromFile(const str& filename, bool flipVertically = false);
        bool loadFromMemory(const void* data, size_t size, bool flipVertically = false);
        bool create(u32 width, u32 height, GLenum internalFormat);

        void bind(u32 slot = 0) const;
        void unbind() const;
        void bindImage(u32 unit, GLenum access, GLenum format) const;
        void resize(u32 width, u32 height);
        void setWrapMode(GLenum mode) const;
        void setFilterMode(GLenum min, GLenum mag) const;

        const void* getData() const;
        size_t getSize() const;
        u32 getWidth() const;
        u32 getHeight() const;
        u32 getChannels() const;
        GLenum getFormat() const;
        GLenum getInternalFormat() const;
        u32 getId() const;
        GLenum getType() const;

    private:
        std::unique_ptr<BinaryData> _data;
        size_t _size = 0;
        i32 _width, _height, _channels;
        // OpenGL stuff
        u32 _textureId         = 0;
        GLenum _internalFormat = GL_RGB, _format = GL_RGB;
        GLenum _type = GL_TEXTURE_2D;

        static GLenum getFormatFromInternal(GLenum internal);
        static GLenum getTypeFromInternal(GLenum internal);
        static u32 getChannelCount(GLenum format);
        void release();
    };
}  // namespace x::Graphics