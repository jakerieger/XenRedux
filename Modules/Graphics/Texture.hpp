// Author: Jake Rieger
// Created: 12/23/2024.
//

#pragma once

#include <glad.h>

#include "Panic.hpp"
#include "Types.hpp"
#include "Data/BinaryData.hpp"

namespace x::Graphics {
    class Texture {
    public:
        Texture() = default;
        ~Texture();

        Texture(const Texture&)            = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&&)                 = delete;
        Texture& operator=(Texture&&)      = delete;

        bool loadFromFile(const str& filename, bool flipVertically = false);
        bool loadFromMemory(const void* data, size_t size, bool flipVertically = false);

        bool create1d() {
            Panic("Unimplemented");
        }

        bool create2d(u32 width, u32 height, GLenum internalFormat);

        bool create3d() {
            Panic("Unimplemented");
        }

        bool createCubemap() {
            Panic("Unimplemented");
        }

        void bind(u32 slot = 0) const;
        void unbind() const;

        void bindImage(u32 unit, GLenum access, GLenum format) const;

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