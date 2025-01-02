// Author: Jake Rieger
// Created: 1/2/2025.
//

#include "IBLPreprocessor.hpp"
#include "Graphics/DebugOpenGL.hpp"

namespace x {
    IBLPreprocessor::IBLPreprocessor(const Filesystem::Path& hdr) : _hdrPath(hdr) {
        std::cout << "Loading HDR: " << _hdrPath.string() << std::endl;

        _equirectToCubemapShader = ShaderManager::instance().getShaderProgram("", "");
        _irradianceShader        = ShaderManager::instance().getShaderProgram("", "");
        _prefilterShader         = ShaderManager::instance().getShaderProgram("", "");
        _brdfShader              = ShaderManager::instance().getShaderProgram("", "");

        if (!_equirectToCubemapShader or !_irradianceShader or !_prefilterShader or !_brdfShader) {
            throw std::runtime_error("Failed to initialize one or more shaders.");
        }

        setupCubemapCapture();
    }

    IBLPreprocessor::~IBLPreprocessor() {
        if (_captureVAO) glDeleteVertexArrays(1, &_captureVAO);
        if (_captureVBO) glDeleteBuffers(1, &_captureVBO);

        _equirectToCubemapShader.reset();
        _irradianceShader.reset();
        _prefilterShader.reset();
        _brdfShader.reset();
    }

    void IBLPreprocessor::generateIBLTextures(const str& outputDir, const Settings& settings) {}

    u32 IBLPreprocessor::convertEquirectangularToCubemap(i32 size) {
        return 0;
    }

    u32 IBLPreprocessor::generateIrradianceMap(u32 envCubemap, i32 size) {
        return 0;
    }

    u32 IBLPreprocessor::generatePrefilterMap(u32 envCubemap, i32 size, i32 mipLevels) {
        return 0;
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cubeVertices), (void*)0);
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

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            glDeleteFramebuffers(1, &captureFBO);
            glDeleteRenderbuffers(1, &captureRBO);
            CHECK_GL_ERROR();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            CHECK_GL_ERROR();
        }

        return captureFBO;
    }
}  // namespace x