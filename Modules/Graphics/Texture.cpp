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
        glBindTexture(_target, _textureId);
        if (CHECK_GL_ERROR()) {
            stbi_image_free(data);
            return false;
        }

        if (_target == GL_TEXTURE_CUBE_MAP) {
            int faceSize = _width / 4;
            if (faceSize != _height / 3) {
                stbi_image_free(data);
                return false;
            }
            const size_t dataSize = _width * _height * _channels;
            std::vector<u8> temp(dataSize);
            std::copy(data, data + dataSize, temp.begin());
            stbi_image_free(data);
            auto faces = extractCubemapFaces(faceSize, temp);

            glGenTextures(1, &_textureId);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _textureId);
            for (int i = 0; i < faces.size(); i++) {
                const auto& face = faces[i];
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0,
                             GL_RGB32F,
                             face.width,
                             face.height,
                             0,
                             GL_RGB,
                             GL_UNSIGNED_BYTE,
                             face.data.data());
                if (CHECK_GL_ERROR()) {
                    release();
                    return false;
                }
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            return true;
        }

        glTexImage2D(_target,
                     0,
                     _internalFormat,
                     _width,
                     _height,
                     0,
                     _format,
                     GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(_target);
        if (CHECK_GL_ERROR()) {
            stbi_image_free(data);
            return false;
        }

        stbi_image_free(data);
        return true;
    }

    bool Texture::loadFromMemory(const void* data, size_t size, bool flipVertically) {
        stbi_set_flip_vertically_on_load(flipVertically);

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

        // TODO: IMPLEMENT CUBE MAPS

        glGenTextures(1, &_textureId);
        glBindTexture(_target, _textureId);
        if (CHECK_GL_ERROR()) { return false; }

        glTexImage2D(_target,
                     0,
                     _internalFormat,
                     _width,
                     _height,
                     0,
                     _format,
                     GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(_target);
        if (CHECK_GL_ERROR()) { return false; }

        return true;
    }

    bool Texture::create(u32 width, u32 height, GLenum internalFormat, GLenum format) {
        release();

        _width          = width;
        _height         = height;
        _internalFormat = internalFormat;
        _format         = format;

        // TODO: IMPLEMENT CUBE MAPS
        if (_target == GL_TEXTURE_CUBE_MAP) {
            glGenTextures(1, &_textureId);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _textureId);
            for (int i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0,
                             _internalFormat,
                             width,
                             height,
                             0,
                             _format,
                             getTypeFromInternal(_internalFormat),
                             nullptr);
                if (CHECK_GL_ERROR()) {
                    release();
                    return false;
                }
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            return true;
        }

        glGenTextures(1, &_textureId);
        glBindTexture(_target, _textureId);
        if (CHECK_GL_ERROR()) {
            glDeleteTextures(1, &_textureId);
            return false;
        }

        glTexImage2D(_target,
                     0,
                     _internalFormat,
                     _width,
                     _height,
                     0,
                     _format,
                     getTypeFromInternal(internalFormat),
                     nullptr);
        if (CHECK_GL_ERROR()) {
            glDeleteTextures(1, &_textureId);
            return false;
        }

        setWrapMode(GL_REPEAT);
        setFilterMode(GL_LINEAR, GL_LINEAR);
        glBindTexture(_target, 0);

        return true;
    }

    Texture::~Texture() {
        release();
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

    GLenum Texture::getFormat() const {
        return _format;
    }

    GLenum Texture::getInternalFormat() const {
        return _internalFormat;
    }

    u32 Texture::getId() const {
        return _textureId;
    }

    GLenum Texture::getTarget() const {
        return _target;
    }

    void Texture::bind(u32 slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(_target, _textureId);
        CHECK_GL_ERROR();
    }

    void Texture::unbind() const {
        glBindTexture(_target, 0);
        CHECK_GL_ERROR();
    }

    void Texture::bindImage(u32 unit, GLenum access, GLenum format) const {
        glBindImageTexture(unit, _textureId, 0, GL_FALSE, 0, access, format);
    }

    void Texture::setWrapMode(GLenum mode) const {
        glBindTexture(_target, _textureId);
        glTexParameteri(_target, GL_TEXTURE_WRAP_S, mode);
        glTexParameteri(_target, GL_TEXTURE_WRAP_T, mode);
        CHECK_GL_ERROR();
    }

    void Texture::setFilterMode(GLenum min, GLenum mag) const {
        glBindTexture(_target, _textureId);
        glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, min);
        glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, mag);
        CHECK_GL_ERROR();
    }

    void Texture::resize(u32 width, u32 height) {
        _width  = width;
        _height = height;
        glBindTexture(_target, _textureId);
        glTexImage2D(_target,
                     0,
                     _internalFormat,
                     _width,
                     _height,
                     0,
                     _format,
                     getTypeFromInternal(_internalFormat),
                     nullptr);
    }

    GLenum Texture::getFormatFromInternal(GLenum internal) {
        switch (internal) {
            // Red channel
            case GL_R8:
            case GL_R16:
            case GL_R16F:
            case GL_R32F:
                return GL_RED;

            // Red-Green channels
            case GL_RG8:
            case GL_RG16:
            case GL_RG16F:
            case GL_RG32F:
                return GL_RG;

            // Red-Green-Blue channels
            case GL_RGB8:
            case GL_RGB16:
            case GL_RGB16F:
            case GL_RGB32F:
                return GL_RGB;

            // Red-Green-Blue-Alpha channels
            case GL_RGBA8:
            case GL_RGBA16:
            case GL_RGBA16F:
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

    GLenum Texture::getTypeFromInternal(GLenum internal) {
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

    u32 Texture::getChannelCount(GLenum format) {
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

    void Texture::release() {
        if (_textureId) {
            glDeleteTextures(1, &_textureId);
            CHECK_GL_ERROR();
            _textureId = 0;
        }
    }

    std::vector<Texture::FaceData> Texture::extractCubemapFaces(int faceSize,
                                                                const std::vector<u8>& data) const {
        std::vector<std::pair<int, int>> facePositions = {
          {2, 1},  // POSITIVE_X
          {0, 1},  // NEGATIVE_X
          {1, 0},  // POSITIVE_Y
          {1, 2},  // NEGATIVE_Y
          {1, 1},  // POSITIVE_Z
          {3, 1}   // NEGATIVE_Z
        };

        std::vector<FaceData> faces(6);
        for (size_t i = 0; i < facePositions.size(); ++i) {
            const auto& [xOffset, yOffset] = facePositions[i];
            FaceData face;
            face.width  = faceSize;
            face.height = faceSize;
            face.data.resize(faceSize * faceSize * getChannels());

            for (int y = 0; y < faceSize; ++y) {
                int srcY  = y + yOffset * faceSize;
                int destY = y * faceSize * getChannels();

                for (int x = 0; x < faceSize; ++x) {
                    int srcX      = x + xOffset * faceSize;
                    int srcIndex  = (srcY * _width + srcX) * getChannels();
                    int destIndex = destY + x * getChannels();

                    std::copy(data.data() + srcIndex,
                              data.data() + srcIndex + getChannels(),
                              face.data.begin() + destIndex);
                }
            }

            faces[i] = std::move(face);
        }

        return faces;
    }
}  // namespace x::Graphics