// Author: Jake Rieger
// Created: 1/2/2025.
//

#include "IBLPreprocessor.hpp"
#include "Graphics/DebugOpenGL.hpp"

#pragma region Shaders
#include "Graphics/Shaders/Include/EquirectToCubemap_VS.h"
#include "Graphics/Shaders/Include/EquirectToCubemap_FS.h"
#include "Graphics/Shaders/Include/IrradianceMap_VS.h"
#include "Graphics/Shaders/Include/IrradianceMap_FS.h"
#include "Graphics/Shaders/Include/PrefilteredMap_VS.h"
#include "Graphics/Shaders/Include/PrefilteredMap_FS.h"
#pragma endregion

namespace x {
    IBLPreprocessor::IBLPreprocessor(const Filesystem::Path& hdr) : _hdrPath(hdr) {
        std::cout << " -- Loading HDR: " << _hdrPath.string() << std::endl;

        _equirectToCubemapShader =
          ShaderManager::instance().getShaderProgram(EquirectToCubemap_VS_Source,
                                                     EquirectToCubemap_FS_Source);
        _irradianceShader = ShaderManager::instance().getShaderProgram(IrradianceMap_VS_Source,
                                                                       IrradianceMap_FS_Source);
        _prefilterShader  = ShaderManager::instance().getShaderProgram(PrefilteredMap_VS_Source,
                                                                      PrefilteredMap_FS_Source);
        // _brdfShader       = ShaderManager::instance().getShaderProgram("", "");

        if (!_equirectToCubemapShader or !_irradianceShader or
            !_prefilterShader /*or !_brdfShader*/) {
            Panic("Failed to initialize one or more shaders.");
        }

        setupCubemapCapture();
        std::cout << " -- IBLGen is ready to generate texture maps." << std::endl;
    }

    IBLPreprocessor::~IBLPreprocessor() {
        if (_captureVAO) glDeleteVertexArrays(1, &_captureVAO);
        if (_captureVBO) glDeleteBuffers(1, &_captureVBO);

        _equirectToCubemapShader.reset();
        _irradianceShader.reset();
        _prefilterShader.reset();
        // _brdfShader.reset();
    }

    IBLTextureHandles IBLPreprocessor::generateIBLTextures(const Settings& settings,
                                                           bool exportToDisk,
                                                           const str& outputDir) {
        u32 envCubemap = convertEquirectangularToCubemap(settings.cubemapSize);
        if (!envCubemap) { Panic("Failed to convert Equirectangular to Cubemap"); }

        u32 irradianceMap = generateIrradianceMap(envCubemap, settings.irradianceSize);
        if (!irradianceMap) { Panic("Failed to generate irradiance map"); }

        u32 prefilterMap =
          generatePrefilterMap(envCubemap, settings.prefilterSize, settings.prefilterMipLevels);
        if (!prefilterMap) { Panic("Failed to generate prefilter map"); }

        return {envCubemap, irradianceMap, prefilterMap, 0};
    }

    u32 IBLPreprocessor::convertEquirectangularToCubemap(i32 size) {
        stbi_set_flip_vertically_on_load(true);
        i32 width, height, channels;
        f32* data = stbi_loadf(_hdrPath.cStr(), &width, &height, &channels, 0);
        if (!data) { throw std::runtime_error("Failed to load image."); }

        u32 hdrTexture;
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        u32 envCubemap;
        glGenTextures(1, &envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        for (u32 i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_RGB16F,
                         size,
                         size,
                         0,
                         GL_RGB,
                         GL_FLOAT,
                         nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        u32 captureFBO = createFramebuffer();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            glDeleteTextures(1, &hdrTexture);
            glDeleteTextures(1, &envCubemap);
            glDeleteFramebuffers(1, &captureFBO);
            CHECK_GL_ERROR();
            Panic("Failed to create framebuffer.");
        }

        // Setup equirect shader
        _equirectToCubemapShader->use();
        _equirectToCubemapShader->setInt("uHDR", 0);
        _equirectToCubemapShader->setMat4("uProjection", kCaptureProjection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glViewport(0, 0, size, size);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

        // render each cubemap face
        for (u32 i = 0; i < 6; i++) {
            _equirectToCubemapShader->setMat4("uView", kCaptureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                   envCubemap,
                                   0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindVertexArray(_captureVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glDeleteTextures(1, &hdrTexture);
        glDeleteFramebuffers(1, &captureFBO);

        return envCubemap;
    }

    u32 IBLPreprocessor::generateIrradianceMap(u32 envCubemap, i32 size) {
        u32 irradianceMap;
        glGenTextures(1, &irradianceMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        for (u32 i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_RGB16F,
                         size,
                         size,
                         0,
                         GL_RGB,
                         GL_FLOAT,
                         nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        u32 captureFBO = createFramebuffer();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            glDeleteTextures(1, &irradianceMap);
            glDeleteFramebuffers(1, &captureFBO);
            CHECK_GL_ERROR();
            Panic("Failed to create framebuffer.");
        }

        _irradianceShader->use();
        _irradianceShader->setInt("uEnvironmentMap", 0);
        _irradianceShader->setMat4("uProjection", kCaptureProjection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glViewport(0, 0, size, size);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

        for (u32 i = 0; i < 6; i++) {
            _irradianceShader->setMat4("uView", kCaptureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                   irradianceMap,
                                   0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Render cube
            glBindVertexArray(_captureVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &captureFBO);

        return irradianceMap;
    }

    u32 IBLPreprocessor::generatePrefilterMap(u32 envCubemap, i32 size, i32 mipLevels) {
        u32 prefilterMap;
        glGenTextures(1, &prefilterMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

        for (u32 i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_RGB16F,
                         size,
                         size,
                         0,
                         GL_RGB,
                         GL_FLOAT,
                         nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // we're using trilinear filtering for the mipmaps
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // ggen mipmaps first
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        _prefilterShader->use();
        _prefilterShader->setInt("uEnvironmentMap", 0);
        _prefilterShader->setMat4("uProjection", kCaptureProjection);

        u32 captureFBO = createFramebuffer();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        for (u32 mip = 0; mip < mipLevels; mip++) {
            // Resize framebuffer according to mip level size
            u32 mipSize = size * std::pow(0.5, mip);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipSize, mipSize);
            glViewport(0, 0, mipSize, mipSize);

            // Calculate roughness for this mip level
            float roughness = (float)mip / (float)(mipLevels - 1);
            _prefilterShader->setFloat("uRoughness", roughness);

            // Render each cube face
            for (u32 i = 0; i < 6; i++) {
                _prefilterShader->setMat4("uView", kCaptureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                                       GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                       prefilterMap,
                                       mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glBindVertexArray(_captureVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &captureFBO);

        return prefilterMap;
    }

    u32 IBLPreprocessor::generateBRDFLUT(i32 size) {
        return 0;
    }

    void IBLPreprocessor::saveCubemapToDisk(u32 cubemap, i32 size, const str& basename) {}

    void IBLPreprocessor::save2DTextureToDisk(u32 texture, i32 size, const str& filename) {}

    void IBLPreprocessor::setupCubemapCapture() {
        glGenVertexArrays(1, &_captureVAO);
        glGenBuffers(1, &_captureVBO);
        glBindVertexArray(_captureVAO);
        glBindBuffer(GL_ARRAY_BUFFER, _captureVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeVertices), kCubeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
        glBindVertexArray(0);
        CHECK_GL_ERROR();
    }

    u32 IBLPreprocessor::createFramebuffer() {
        u32 captureFBO, captureRBO;
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);
        CHECK_GL_ERROR();

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        CHECK_GL_ERROR();

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
        CHECK_GL_ERROR();

        return captureFBO;
    }
}  // namespace x