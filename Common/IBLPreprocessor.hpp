// Author: Jake Rieger
// Created: 1/2/2025.
//

#pragma once

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Panic.hpp"
#include "ShaderManager.hpp"
#include "Graphics/ShaderProgram.hpp"
#include "Filesystem/Filesystem.hpp"

namespace {
    // Capture view matrices for each face of the cubemap
    const glm::mat4 captureViews[] {
      glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    };

    constexpr f32 cubeVertices[] = {
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,
    };
}  // namespace

namespace x {

    /**
     * @brief Handles the preprocessing of HDR environment maps for Image-Based Lighting (IBL).
     *
     * This class converts equirectangular HDR environment maps into the various texture formats
     * needed for physically based rendering with image-based lighting. It generates:
     * - A cubemap environment map
     * - A diffuse irradiance cubemap for ambient lighting
     * - A pre-filtered environment cubemap for specular reflections
     * - A BRDF Look-Up Texture (LUT) for efficient rendering
     */
    class IBLPreprocessor {
    public:
        /**
         * @brief Configuration settings for IBL texture generation.
         */
        struct Settings {
            i32 cubemapSize        = 1024;
            i32 irradianceSize     = 32;
            i32 prefilterSize      = 128;
            i32 brdfLUTSize        = 512;
            i32 prefilterMipLevels = 5;
        };

        /**
         * @brief Constructs an IBL preprocessor for the specified HDR environment map.
         * @param hdrPath Path to the equirectangular HDR environment map.
         * @throws std::runtime_error if shader compilation fails or HDR loading fails.
         */
        explicit IBLPreprocessor(const Filesystem::Path& hdr);

        /**
         * @brief Cleans up OpenGL resources.
         */
        ~IBLPreprocessor();

        /**
         * @brief Generates all required IBL textures and saves them to disk.
         *
         * @param outputDir Directory where the generated textures will be saved.
         * @param settings Optional settings to control texture generation parameters.
         * @throws std::runtime_error if texture generation or saving fails.
         */
        void generateIBLTextures(const str& outputDir, const Settings& settings = Settings());

    private:
        Filesystem::Path _hdrPath;
        std::shared_ptr<Graphics::ShaderProgram>
          _equirectToCubemapShader;  // Converts equirectangular to cubemap
        std::shared_ptr<Graphics::ShaderProgram> _irradianceShader;  // Computes diffuse irradiance
        std::shared_ptr<Graphics::ShaderProgram>
          _prefilterShader;  // Creates prefiltered environment map
        std::shared_ptr<Graphics::ShaderProgram> _brdfShader;  // Generates BRDF LUT

        u32 _captureVAO = 0;  // Vertex Array Object for rendering
        u32 _captureVBO = 0;  // Vertex Buffer Object for cube vertices

        /**
         * @brief Converts equirectangular HDR image to a cubemap.
         *
         * @param size Size of each cubemap face in pixels.
         * @return u32 OpenGL texture ID of the generated cubemap.
         */
        u32 convertEquirectangularToCubemap(i32 size);

        /**
         * @brief Generates a diffuse irradiance cubemap by convolving the environment map.
         *
         * @param envCubemap Source environment cubemap.
         * @param size Size of each irradiance map face in pixels.
         * @return u32 OpenGL texture ID of the generated irradiance map.
         */
        u32 generateIrradianceMap(u32 envCubemap, i32 size);

        /**
         * @brief Creates a pre-filtered environment map for specular reflections.
         *
         * @param envCubemap Source environment cubemap.
         * @param size Base size of the prefiltered map (gets progressively smaller for mips).
         * @param mipLevels Number of mip levels to generate.
         * @return u32 OpenGL texture ID of the generated prefiltered map.
         */
        u32 generatePrefilterMap(u32 envCubemap, i32 size, i32 mipLevels);

        /**
         * @brief Generates a BRDF Look-Up Texture.
         *
         * @param size Size of the 2D LUT texture in pixels.
         * @return u32 OpenGL texture ID of the generated BRDF LUT.
         */
        u32 generateBRDFLUT(i32 size);

        /**
         * @brief Saves a cubemap texture to disk as separate face images.
         *
         * @param cubemap OpenGL texture ID of the cubemap to save.
         * @param size Size of each face in pixels.
         * @param basename Base filename for the saved images.
         */
        void saveCubemapToDisk(u32 cubemap, i32 size, const str& basename);

        /**
         * @brief Saves a 2D texture to disk.
         *
         * @param texture OpenGL texture ID of the 2D texture to save.
         * @param size Size of the texture in pixels.
         * @param filename Output filename.
         */
        void save2DTextureToDisk(u32 texture, i32 size, const str& filename);

        /**
         * @brief Sets up OpenGL state for cubemap capture.
         */
        void setupCubemapCapture();

        /**
         * @brief Creates and returns a framebuffer for texture capture.
         *
         * @return u32 OpenGL framebuffer ID.
         */
        u32 createFramebuffer();

        IBLPreprocessor(const IBLPreprocessor&)            = delete;
        IBLPreprocessor& operator=(const IBLPreprocessor&) = delete;
    };
}  // namespace x
