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
        friend class TextureCube;

    public:
        Texture(GLenum target = GL_TEXTURE_2D) : _target(target) {}
        ~Texture();

        Texture(const Texture&)            = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&&)                 = delete;
        Texture& operator=(Texture&&)      = delete;

        virtual bool loadFromFile(const str& filename, bool flipVertically = false);
        virtual bool loadFromMemory(const void* data, size_t size, bool flipVertically = false);
        virtual bool create(u32 width, u32 height, GLenum internalFormat, GLenum format);
        virtual void bind(u32 slot = 0) const;
        virtual void unbind() const;
        virtual void resize(u32 width, u32 height);
        virtual void setWrapMode(GLenum mode) const;
        virtual void setFilterMode(GLenum min, GLenum mag) const;

        void bindImage(u32 unit, GLenum access, GLenum format) const;

        u32 getWidth() const;
        u32 getHeight() const;
        u32 getChannels() const;
        GLenum getFormat() const;
        GLenum getInternalFormat() const;
        u32 getId() const;
        GLenum getTarget() const;

    private:
        i32 _width;
        i32 _height;
        i32 _channels;
        // OpenGL stuff
        u32 _textureId         = 0;
        GLenum _internalFormat = GL_RGB;
        GLenum _format         = GL_RGB;
        GLenum _target;

        static GLenum getFormatFromInternal(GLenum internal);
        static GLenum getTypeFromInternal(GLenum internal);
        static u32 getChannelCount(GLenum format);
        void release();

        struct FaceData {
            int width, height;
            std::vector<u8> data;
        };

        enum CubemapFace {
            POSITIVE_X,
            NEGATIVE_X,
            POSITIVE_Y,
            NEGATIVE_Y,
            POSITIVE_Z,
            NEGATIVE_Z,
        };

        std::vector<FaceData> extractCubemapFaces(int faceSize, const std::vector<u8>& data) const;
    };
}  // namespace x::Graphics