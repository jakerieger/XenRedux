// Author: Jake Rieger
// Created: 12/23/2024.
//

#include <stb_image.h>
#include "Panic.hpp"
#include "Texture.hpp"
#include "DebugOpenGL.hpp"

namespace x::Graphics {
    bool Texture::loadFromFile(const str& filename, bool flipVertically) {
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

    bool Texture::loadFromMemory(const void* data, size_t size, bool flipVertically) {
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

    Texture::~Texture() {
        glDeleteTextures(1, &_textureId);
        CHECK_GL_ERROR();
        _data.reset();
    }

    const void* Texture::getData() const {
        return _data->getData();
    }

    size_t Texture::getSize() const {
        return _data->getSize();
    }

    u32 Texture::getWidth() const {
        return _width;
    }

    u32 Texture::getHeight() const {
        return _height;
    }

    u32 Texture::getChannels() const {
        return _channels;
    }

    void Texture::bind(u32 slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, _textureId);
        CHECK_GL_ERROR();
    }

    void Texture::unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
        CHECK_GL_ERROR();
    }

    void Texture::setWrapMode(GLenum mode) const {
        glBindTexture(GL_TEXTURE_2D, _textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
        CHECK_GL_ERROR();
    }

    void Texture::setFilterMode(GLenum min, GLenum mag) const {
        glBindTexture(GL_TEXTURE_2D, _textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
        CHECK_GL_ERROR();
    }
}  // namespace x::Graphics