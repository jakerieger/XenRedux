// Author: Jake Rieger
// Created: 12/23/2024.
//

#include <stb_image.h>
#include "Panic.hpp"
#include "Texture2D.hpp"
#include "DebugOpenGL.hpp"

namespace x::Graphics {
    bool Texture2D::loadFromFile(const str& filename, bool flipVertically) {
        stbi_set_flip_vertically_on_load(flipVertically);
        unsigned char* data = stbi_load(filename.c_str(), &_width, &_height, &_channels, 0);
        if (!data) { return false; }
        _data = std::make_unique<BinaryData>(data, _width * _height * _channels);
        if (!_data) { return false; }

        // determine internal format
        if (_channels == 1) {
            _internalFormat = GL_R8;
            _format         = GL_RED;
        } else if (_channels == 3) {
            _internalFormat = GL_RGB8;
            _format         = GL_RGB;
        } else if (_channels == 4) {
            _internalFormat = GL_RGBA8;
            _format         = GL_RGBA;
        }

        glGenTextures(1, &_textureId);
        glBindTexture(GL_TEXTURE_2D, _textureId);
        if (CHECK_GL_ERROR()) {
            stbi_image_free(data);
            return false;
        }

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     _internalFormat,
                     _width,
                     _height,
                     0,
                     _format,
                     GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);
        if (CHECK_GL_ERROR()) {
            stbi_image_free(data);
            return false;
        }

        stbi_image_free(data);
        return true;
    }

    bool Texture2D::loadFromMemory(const void* data, size_t size, bool flipVertically) {
        stbi_set_flip_vertically_on_load(flipVertically);
        _data = std::make_unique<BinaryData>(data, size);
        if (!_data) { return false; }

        if (_channels == 1) {
            _internalFormat = GL_R8;
            _format         = GL_RED;
        } else if (_channels == 3) {
            _internalFormat = GL_RGB8;
            _format         = GL_RGB;
        } else if (_channels == 4) {
            _internalFormat = GL_RGBA8;
            _format         = GL_RGBA;
        }

        glGenTextures(1, &_textureId);
        glBindTexture(GL_TEXTURE_2D, _textureId);
        if (CHECK_GL_ERROR()) { return false; }

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     _internalFormat,
                     _width,
                     _height,
                     0,
                     _format,
                     GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);
        if (CHECK_GL_ERROR()) { return false; }

        return true;
    }

    bool Texture2D::create(u32 width, u32 height, GLenum internalFormat) {
        release();

        const GLenum format = getFormatFromInternal(internalFormat);
        const GLenum type   = getTypeFromInternal(internalFormat);

        _width          = width;
        _height         = height;
        _internalFormat = internalFormat;
        _format         = format;
        _type           = type;

        glGenTextures(1, &_textureId);
        glBindTexture(GL_TEXTURE_2D, _textureId);
        if (CHECK_GL_ERROR()) {
            glDeleteTextures(1, &_textureId);
            return false;
        }

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     _internalFormat,
                     _width,
                     _height,
                     0,
                     _format,
                     _type,
                     nullptr);
        if (CHECK_GL_ERROR()) {
            glDeleteTextures(1, &_textureId);
            return false;
        }

        setWrapMode(GL_REPEAT);
        setFilterMode(GL_LINEAR, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    Texture2D::~Texture2D() {
        release();
    }

    const void* Texture2D::getData() const {
        return _data->getData();
    }

    size_t Texture2D::getSize() const {
        return _data->getSize();
    }

    u32 Texture2D::getWidth() const {
        return _width;
    }

    u32 Texture2D::getHeight() const {
        return _height;
    }

    u32 Texture2D::getChannels() const {
        return _channels;
    }

    GLenum Texture2D::getFormat() const {
        return _format;
    }

    GLenum Texture2D::getInternalFormat() const {
        return _internalFormat;
    }

    u32 Texture2D::getId() const {
        return _textureId;
    }

    GLenum Texture2D::getType() const {
        return _type;
    }

    void Texture2D::bind(u32 slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, _textureId);
        CHECK_GL_ERROR();
    }

    void Texture2D::unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
        CHECK_GL_ERROR();
    }

    void Texture2D::bindImage(u32 unit, GLenum access, GLenum format) const {
        glBindImageTexture(unit, _textureId, 0, GL_FALSE, 0, access, format);
    }

    void Texture2D::setWrapMode(GLenum mode) const {
        glBindTexture(GL_TEXTURE_2D, _textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
        CHECK_GL_ERROR();
    }

    void Texture2D::setFilterMode(GLenum min, GLenum mag) const {
        glBindTexture(GL_TEXTURE_2D, _textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
        CHECK_GL_ERROR();
    }

    void Texture2D::resize(u32 width, u32 height) {
        _width  = width;
        _height = height;
        glBindTexture(GL_TEXTURE_2D, _textureId);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     _internalFormat,
                     _width,
                     _height,
                     0,
                     _format,
                     _type,
                     nullptr);
    }

    GLenum Texture2D::getFormatFromInternal(GLenum internal) {
        switch (internal) {
            // Red channel
            case GL_R8:
            case GL_R16:
            case GL_R32F:
                return GL_RED;

            // Red-Green channels
            case GL_RG8:
            case GL_RG16:
            case GL_RG32F:
                return GL_RG;

            // Red-Green-Blue channels
            case GL_RGB8:
            case GL_RGB16:
            case GL_RGB32F:
                return GL_RGB;

            // Red-Green-Blue-Alpha channels
            case GL_RGBA8:
            case GL_RGBA16:
            case GL_RGBA32F:
                return GL_RGBA;

            // Depth and Depth-Stencil formats
            case GL_DEPTH_COMPONENT16:
            case GL_DEPTH_COMPONENT24:
            case GL_DEPTH_COMPONENT32:
                return GL_DEPTH_COMPONENT;

            case GL_DEPTH24_STENCIL8:
            case GL_DEPTH32F_STENCIL8:
                return GL_DEPTH_STENCIL;

            default:
                return GL_NONE;  // Invalid format
        }
    }

    GLenum Texture2D::getTypeFromInternal(GLenum internal) {
        switch (internal) {
            case GL_R8:
            case GL_RG8:
            case GL_RGB8:
            case GL_RGBA8:
                return GL_UNSIGNED_BYTE;

            case GL_R8I:
            case GL_RG8I:
            case GL_RGB8I:
            case GL_RGBA8I:
                return GL_BYTE;

            case GL_R16:
            case GL_RG16:
            case GL_RGB16:
            case GL_RGBA16:
                return GL_UNSIGNED_SHORT;

            case GL_R16F:
            case GL_RG16F:
            case GL_RGB16F:
            case GL_RGBA16F:
                return GL_HALF_FLOAT;

            case GL_R32F:
            case GL_RG32F:
            case GL_RGB32F:
            case GL_RGBA32F:
                return GL_FLOAT;

            case GL_R16I:
            case GL_RG16I:
            case GL_RGB16I:
            case GL_RGBA16I:
                return GL_SHORT;

            case GL_R32I:
            case GL_RG32I:
            case GL_RGB32I:
            case GL_RGBA32I:
                return GL_INT;

            case GL_R16UI:
            case GL_RG16UI:
            case GL_RGB16UI:
            case GL_RGBA16UI:
                return GL_UNSIGNED_SHORT;

            case GL_R32UI:
            case GL_RG32UI:
            case GL_RGB32UI:
            case GL_RGBA32UI:
                return GL_UNSIGNED_INT;

            case GL_R11F_G11F_B10F:
                return GL_FLOAT;

            case GL_RGB10_A2:
            case GL_RGB10_A2UI:
                return GL_UNSIGNED_INT_2_10_10_10_REV;

            default:
                return GL_NONE;  // Invalid format
        }
    }

    u32 Texture2D::getChannelCount(GLenum format) {
        switch (format) {
            case GL_RED:
                return 1;
            case GL_RG:
                return 2;
            case GL_RGB:
                return 3;
            case GL_RGBA:
                return 4;
            default:
                return 0;
        }
    }

    void Texture2D::release() {
        if (_textureId) {
            glDeleteTextures(1, &_textureId);
            CHECK_GL_ERROR();
            _textureId = 0;
            _data.reset();
        }
    }
}  // namespace x::Graphics