// Author: Jake Rieger
// Created: 12/30/2024.
//

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <imgui.h>
#include <memory>
#include <array>
#include <tinyfiledialogs.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfFrameBuffer.h>
#include <Imath/ImathBox.h>

// XEN includes
#include <Panic.hpp>
#include <Types.hpp>
#include <Game.hpp>
#include <ShaderManager.hpp>
#include <Graphics/ShaderProgram.hpp>
#include <Filesystem/Filesystem.hpp>

using namespace x::Filesystem;
using namespace x::Graphics;

#pragma region Shaders
#include <Graphics/Shaders/Include/Skybox_VS.h>
#include <Graphics/Shaders/Include/Skybox_FS.h>
#include <Graphics/Shaders/Include/Unlit_VS.h>
#include <Graphics/Shaders/Include/Unlit_FS.h>
#include <Graphics/Shaders/Include/BRDF_LUT_CS.h>
#include <Graphics/Shaders/Include/IrradianceMap_CS.h>
#include <Graphics/Shaders/Include/PrefilterMap_CS.h>
#pragma endregion

static constexpr i32 kWidth                = 1600;
static constexpr i32 kHeight               = 900;
static constexpr i32 kCubeFaces            = 6;
static constexpr i32 kSkyboxResolution     = 512;  // Each face
static constexpr i32 kBrdfResolution       = 512;  // 512x512
static constexpr i32 kIrradianceResolution = 32;   // 32x32
static constexpr i32 kPrefilterResolution  = 128;  // 128x128

static constexpr f32 kViewportFaceSize  = 48.0f;
static constexpr f32 kCubeVertices[108] = {
  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,
  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,
  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f,
  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f,
  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

enum Cubemaps {
    Skybox,
    Irradiance,
    Prefilter,
};

class IBLGen final : public x::IGame {
public:
    IBLGen() : IGame("IBL Gen", kWidth, kHeight, true, false) {}

    void loadContent() override {
        // Create the cube for viewing cubemaps
        glGenVertexArrays(1, &_cubeVAO);
        glBindVertexArray(_cubeVAO);
        glGenBuffers(1, &_cubeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, _cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeVertices), kCubeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        _skyboxShader =
          x::ShaderManager::get().getShaderProgram(Skybox_VS_Source, Skybox_FS_Source);
        if (!_skyboxShader) { Panic("Failed to load Skybox shader"); }
        _skyboxShader->use();
        _skyboxShader->setInt("uSkybox", 0);

        _brdfLutShader = x::ShaderManager::get().getShaderProgram(BRDF_LUT_CS_Source);
        if (!_brdfLutShader.get()) { Panic("Failed to load BRDF LUT shader"); }
        _irradianceShader = x::ShaderManager::get().getShaderProgram(IrradianceMap_CS_Source);
        if (!_irradianceShader.get()) { Panic("Failed to load Irradiance shader"); }
        _prefilterShader = x::ShaderManager::get().getShaderProgram(PrefilterMap_CS_Source);
        if (!_prefilterShader.get()) { Panic("Failed to load Prefilter shader"); }

        createCubemapTexture(_skyboxMap, 512, GL_RGB16F, GL_RGB, false);
        loadCubemap(R"(C:\Users\conta\Code\XenRedux\Tools\IBLGen\Data\test_sky.hdr)", _skyboxMap);
    }

    void unloadContent() override {
        glDeleteVertexArrays(1, &_cubeVAO);
        glDeleteVertexArrays(1, &_quadVAO);

        glDeleteBuffers(1, &_cubeVBO);
        glDeleteBuffers(1, &_quadVBO);
        glDeleteBuffers(1, &_quadIBO);

        glDeleteTextures(1, &_skyboxMap);
        glDeleteTextures(1, &_brdfLut);
        glDeleteTextures(1, &_irradianceMap);
        glDeleteTextures(1, &_prefilterMap);

        if (_debugFBOs[Skybox]) { glDeleteFramebuffers(1, &_debugFBOs[Skybox]); }
        if (_debugSkyboxFaces[0]) {
            glDeleteFramebuffers(kCubeFaces, _debugSkyboxFaces);
            for (i32 i = 0; i < kCubeFaces; ++i) {
                _debugSkyboxFaces[i] = 0;
            }
        }

        _skyboxShader.reset();
    }

    void update() override {
        glm::mat4 v =
          glm::lookAt(glm::vec3(0.0, 0.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 p =
          glm::perspective(glm::radians(45.0f), (f32)kWidth / (f32)kHeight, 0.1f, 100.0f);

        _skyboxShader->use();
        _skyboxShader->setMat4("uVP", p * glm::mat4(glm::mat3(v)));
    }

    void draw() override {
        x::Context::clear(true);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        // Draw cubemap box here
        _skyboxShader->use();
        glBindVertexArray(_cubeVAO);

        if (_skyboxMap) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _skyboxMap);
        }

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
    }

    void configurePipeline() override {}

    void drawDebugUI() override {
        ImGui::Begin("Settings",
                     nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
        if (ImGui::Button("Load HDR Cubemap")) {
            const char* title      = "Load HDR Cubemap";
            const char* patterns[] = {"*.hdr"};
            const char* filename   = tinyfd_openFileDialog(title, "", 1, patterns, nullptr, 0);
            if (filename) { loadCubemap(filename, _skyboxMap); }
        }

        if (ImGui::Button("Generate BRDF LUT")) { generateBRDF(); }

        if (ImGui::Button("Generate Irradiance Map")) { generateIrradiance(); }

        if (ImGui::Button("Generate Prefilter")) { generatePrefilter(); }

        if (ImGui::Button("Export Maps")) { exportMaps(); }

        ImGui::Separator();

        if (_skyboxMap) {
            ImGui::Text("Skybox");
            if (!_debugSkyboxFaces[0]) setupDebugCubemapFaces(Cubemaps::Skybox);
            updateDebugCubemapFaces(_skyboxMap, kSkyboxResolution, Cubemaps::Skybox);
            f32 faceSize   = kViewportFaceSize;
            ImVec2 basePos = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(basePos.x + faceSize, basePos.y));
            ImGui::Image((ImTextureID)(intptr_t)_debugSkyboxFaces[2], ImVec2(faceSize, faceSize));
            ImGui::SetCursorPos(ImVec2(basePos.x, basePos.y + faceSize));
            for (int i = 0; i < 4; i++) {
                int faceIdx = (i == 0) ? 1 : (i == 1) ? 4 : (i == 2) ? 0 : 5;  // -X, +Z, +X, -Z
                ImGui::Image((ImTextureID)(intptr_t)_debugSkyboxFaces[faceIdx],
                             ImVec2(faceSize, faceSize));
                if (i < 3) {
                    // Calculate exact position for the next image
                    ImGui::SameLine(basePos.x + (i + 1) * faceSize, 0.0f);
                }
            }
            ImGui::SetCursorPos(ImVec2(basePos.x + faceSize, basePos.y + 2 * faceSize));
            ImGui::Image((ImTextureID)(intptr_t)_debugSkyboxFaces[3], ImVec2(faceSize, faceSize));
        }

        if (_brdfLut) {
            ImGui::Text("BRDF LUT");
            ImVec2 size(200.f, 200.f);
            ImGui::Image((ImTextureID)(int*)_brdfLut, size, ImVec2(0, 1), ImVec2(1, 0));
        }

        if (_irradianceMap) {
            ImGui::Text("Irradiance Map");
            if (!_debugIrradianceFaces[0]) setupDebugCubemapFaces(Cubemaps::Irradiance);
            updateDebugCubemapFaces(_irradianceMap, kIrradianceResolution, Cubemaps::Irradiance);
            f32 faceSize   = kViewportFaceSize;
            ImVec2 basePos = ImGui::GetCursorPos();

            // Draw the faces in a cross pattern
            //     [+Y]
            // [-X][+Z][+X][-Z]
            //     [-Y]

            // Top face (+Y)
            ImGui::SetCursorPos(ImVec2(basePos.x + faceSize, basePos.y));
            ImGui::Image((ImTextureID)(intptr_t)_debugIrradianceFaces[2],
                         ImVec2(faceSize, faceSize));

            // Middle row
            ImGui::SetCursorPos(ImVec2(basePos.x, basePos.y + faceSize));
            for (int i = 0; i < 4; i++) {
                int faceIdx = (i == 0) ? 1 : (i == 1) ? 4 : (i == 2) ? 0 : 5;  // -X, +Z, +X, -Z
                ImGui::Image((ImTextureID)(intptr_t)_debugIrradianceFaces[faceIdx],
                             ImVec2(faceSize, faceSize));
                if (i < 3) {
                    // Calculate exact position for the next image
                    ImGui::SameLine(basePos.x + (i + 1) * faceSize, 0.0f);
                }
            }

            // Bottom face (-Y)
            ImGui::SetCursorPos(ImVec2(basePos.x + faceSize, basePos.y + 2 * faceSize));
            ImGui::Image((ImTextureID)(intptr_t)_debugIrradianceFaces[3],
                         ImVec2(faceSize, faceSize));
        }

        if (_prefilterMap) {
            ImGui::Text("Prefilter Map");
            if (!_debugPrefilterFaces[0]) setupDebugCubemapFaces(Cubemaps::Prefilter);
            updateDebugCubemapFaces(_prefilterMap, kPrefilterResolution, Cubemaps::Prefilter);
            f32 faceSize   = kViewportFaceSize;
            ImVec2 basePos = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(basePos.x + faceSize, basePos.y));
            ImGui::Image((ImTextureID)(intptr_t)_debugPrefilterFaces[2],
                         ImVec2(faceSize, faceSize));
            ImGui::SetCursorPos(ImVec2(basePos.x, basePos.y + faceSize));
            for (int i = 0; i < 4; i++) {
                int faceIdx = (i == 0) ? 1 : (i == 1) ? 4 : (i == 2) ? 0 : 5;  // -X, +Z, +X, -Z
                ImGui::Image((ImTextureID)(intptr_t)_debugPrefilterFaces[faceIdx],
                             ImVec2(faceSize, faceSize));
                if (i < 3) {
                    // Calculate exact position for the next image
                    ImGui::SameLine(basePos.x + (i + 1) * faceSize, 0.0f);
                }
            }
            ImGui::SetCursorPos(ImVec2(basePos.x + faceSize, basePos.y + 2 * faceSize));
            ImGui::Image((ImTextureID)(intptr_t)_debugPrefilterFaces[3],
                         ImVec2(faceSize, faceSize));
        }

        ImGui::End();
    }

    void onKeyDown(u16 key) override {}
    void onKeyUp(u16 key) override {}
    void onMouseMove(i32 x, i32 y) override {}
    void onMouseDown(u16 button, i32 x, i32 y) override {}
    void onMouseUp(u16 button, i32 x, i32 y) override {}

private:
    u32 _cubeVAO       = 0;
    u32 _cubeVBO       = 0;
    u32 _quadVAO       = 0;
    u32 _quadVBO       = 0;
    u32 _quadIBO       = 0;
    u32 _brdfLut       = 0;
    u32 _irradianceMap = 0;
    u32 _prefilterMap  = 0;
    u32 _skyboxMap     = 0;
    std::shared_ptr<ShaderProgram> _skyboxShader;
    std::shared_ptr<ShaderProgram> _brdfLutShader;
    std::shared_ptr<ShaderProgram> _irradianceShader;
    std::shared_ptr<ShaderProgram> _prefilterShader;
    u32 _debugFBOs[3]            = {0};
    u32 _debugSkyboxFaces[6]     = {0};
    u32 _debugIrradianceFaces[6] = {0};
    u32 _debugPrefilterFaces[6]  = {0};

    struct FaceData {
        int width, height;
        std::vector<u8> data;

        f32* getFloatData() {
            return RCAST<f32*>(data.data());
        }

        const f32* getFloatData() const {
            return RCAST<const f32*>(data.data());
        }
    };

    enum CubemapFace {
        POSITIVE_X,
        NEGATIVE_X,
        POSITIVE_Y,
        NEGATIVE_Y,
        POSITIVE_Z,
        NEGATIVE_Z,
    };

    void loadCubemap(const str& filename, const u32 cubemap) {
        i32 width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        f32* data = stbi_loadf(filename.c_str(), &width, &height, &channels, 3);
        if (!data) { Panic("Couldn't load image"); }
        auto faceSize = width / 4;
        auto faces    = extractCubemapFaces(faceSize, data);
        for (int i = 0; i < faces.size(); i++) {
            updateCubemapFace(cubemap, i, 0, faceSize, faceSize, faces[i].data.data());
        }
        stbi_image_free(data);
    }

    void generateBRDF() {
        if (_brdfLut) { glDeleteTextures(1, &_brdfLut); }
        glGenTextures(1, &_brdfLut);
        glBindTexture(GL_TEXTURE_2D, _brdfLut);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RG16F,
                     kBrdfResolution,
                     kBrdfResolution,
                     0,
                     GL_RG,
                     GL_FLOAT,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        _brdfLutShader->use();
        glBindImageTexture(0, _brdfLut, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG16F);
        const auto workgroupSize =
          ShaderProgram::getComputeWorkGroupSize(16, kBrdfResolution, kBrdfResolution);
        _brdfLutShader->dispatchCompute(workgroupSize.first, workgroupSize.second, 1);
    }

    void generateIrradiance() {
        if (!_skyboxMap) { Panic("Environment map has not been loaded."); }
        if (_irradianceMap) { glDeleteTextures(1, &_irradianceMap); }
        createCubemapTexture(_irradianceMap, kIrradianceResolution, GL_RGBA16F, GL_RGBA);

        u32 fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        u32 tempTexture;
        glGenTextures(1, &tempTexture);
        glBindTexture(GL_TEXTURE_2D, tempTexture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA16F,
                     kIrradianceResolution,
                     kIrradianceResolution,
                     0,
                     GL_RGBA,
                     GL_FLOAT,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        struct IrradianceParams {
            glm::mat4 viewMatrix;
            i32 faceIndex;
            f32 deltaPhi;
            f32 deltaTheta;
            i32 numSamples;
        };

        u32 ubo;
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(IrradianceParams), nullptr, GL_DYNAMIC_DRAW);

        IrradianceParams params;
        params.deltaPhi   = glm::two_pi<f32>() / 180.0f;  // 2° steps
        params.deltaTheta = glm::half_pi<f32>() / 64.0f;  // ~1.4° steps
        params.numSamples = 1024;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _skyboxMap);

        _irradianceShader->use();
        _irradianceShader->setInt("uEnvironmentMap", 0);

        for (u32 face = 0; face < kCubeFaces; ++face) {
            params.faceIndex = face;
            // Calculate view matrix for current face
            switch (face) {
                case 0:  // POSITIVE_X
                    params.viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                                    glm::vec3(1.0f, 0.0f, 0.0f),
                                                    glm::vec3(0.0f, -1.0f, 0.0f));
                    break;
                case 1:  // NEGATIVE_X
                    params.viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                                    glm::vec3(-1.0f, 0.0f, 0.0f),
                                                    glm::vec3(0.0f, -1.0f, 0.0f));
                    break;
                case 2:  // POSITIVE_Y
                    params.viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                                    glm::vec3(0.0f, 1.0f, 0.0f),
                                                    glm::vec3(0.0f, 0.0f, 1.0f));
                    break;
                case 3:  // NEGATIVE_Y
                    params.viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                                    glm::vec3(0.0f, -1.0f, 0.0f),
                                                    glm::vec3(0.0f, 0.0f, -1.0f));
                    break;
                case 4:  // POSITIVE_Z
                    params.viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                                    glm::vec3(0.0f, 0.0f, 1.0f),
                                                    glm::vec3(0.0f, -1.0f, 0.0f));
                    break;
                case 5:  // NEGATIVE_Z
                    params.viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                                    glm::vec3(0.0f, 0.0f, -1.0f),
                                                    glm::vec3(0.0f, -1.0f, 0.0f));
                    break;
            }

            glBindImageTexture(1, tempTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
            glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(IrradianceParams), &params);
            const auto workgroupSize =
              x::Graphics::ShaderProgram::getComputeWorkGroupSize(32,
                                                                  kIrradianceResolution,
                                                                  kIrradianceResolution);
            _irradianceShader->dispatchCompute(workgroupSize.first, workgroupSize.second, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            glCopyImageSubData(tempTexture,
                               GL_TEXTURE_2D,
                               0,
                               0,
                               0,
                               0,
                               _irradianceMap,
                               GL_TEXTURE_CUBE_MAP,
                               0,
                               0,
                               0,
                               face,
                               kIrradianceResolution,
                               kIrradianceResolution,
                               1);
        }

        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &tempTexture);
        glDeleteBuffers(1, &ubo);

        // // Generate mips if needed
        // glBindTexture(GL_TEXTURE_CUBE_MAP, _irradianceMap);
        // glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    void generatePrefilter() {
        if (!_skyboxMap) {
            Panic("Environment map has not been loaded.");
            return;
        }

        // Create or recreate the prefilter map with mips
        if (_prefilterMap) { glDeleteTextures(1, &_prefilterMap); }
        createCubemapTexture(_prefilterMap, kPrefilterResolution, GL_RGBA16F, GL_RGBA, true);

        // Create temporary framebuffer and 2D texture for compute shader output
        u32 fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        u32 tempTexture;
        glGenTextures(1, &tempTexture);
        glBindTexture(GL_TEXTURE_2D, tempTexture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA16F,
                     kPrefilterResolution,
                     kPrefilterResolution,
                     0,
                     GL_RGB,
                     GL_FLOAT,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Parameters for the compute shader
        struct PrefilterParams {
            glm::mat4 viewMatrix;
            i32 faceIndex;
            f32 roughness;
            i32 numSamples;
            f32 resolution;
        };

        // Create UBO for parameters
        u32 ubo;
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(PrefilterParams), nullptr, GL_DYNAMIC_DRAW);

        // Precompute number of mip levels
        const u32 maxMipLevels = CAST<u32>(std::floor(std::log2(kPrefilterResolution))) + 1;

        // Bind input environment map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _skyboxMap);

        _prefilterShader->use();
        _prefilterShader->setInt("uEnvironmentMap", 0);

        // Process each mip level
        for (u32 mip = 0; mip < maxMipLevels; ++mip) {
            // Calculate current mip dimension
            const u32 mipWidth  = kPrefilterResolution >> mip;
            const u32 mipHeight = mipWidth;  // Keep it square

            // Skip if mip dimension becomes too small
            if (mipWidth < 1) break;

            // Roughness increases with mip level
            const float roughness = CAST<f32>(mip) / CAST<f32>(maxMipLevels - 1);

            PrefilterParams params;
            params.numSamples = 1024;
            params.roughness  = roughness;
            params.resolution = CAST<f32>(mipWidth);

            // Process each face
            for (u32 face = 0; face < kCubeFaces; ++face) {
                params.faceIndex = face;

                // Calculate view matrix for current face
                switch (face) {
                    case 0:  // POSITIVE_X
                        params.viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                                        glm::vec3(1.0f, 0.0f, 0.0f),
                                                        glm::vec3(0.0f, -1.0f, 0.0f));
                        break;
                    case 1:  // NEGATIVE_X
                        params.viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                                        glm::vec3(-1.0f, 0.0f, 0.0f),
                                                        glm::vec3(0.0f, -1.0f, 0.0f));
                        break;
                    case 2:  // POSITIVE_Y
                        params.viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                                        glm::vec3(0.0f, 1.0f, 0.0f),
                                                        glm::vec3(0.0f, 0.0f, 1.0f));
                        break;
                    case 3:  // NEGATIVE_Y
                        params.viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                                        glm::vec3(0.0f, -1.0f, 0.0f),
                                                        glm::vec3(0.0f, 0.0f, -1.0f));
                        break;
                    case 4:  // POSITIVE_Z
                        params.viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                                        glm::vec3(0.0f, 0.0f, 1.0f),
                                                        glm::vec3(0.0f, -1.0f, 0.0f));
                        break;
                    case 5:  // NEGATIVE_Z
                        params.viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                                        glm::vec3(0.0f, 0.0f, -1.0f),
                                                        glm::vec3(0.0f, -1.0f, 0.0f));
                        break;
                }

                // Bind output image
                glBindImageTexture(1, tempTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);

                // Update and bind uniform buffer
                glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo);
                glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PrefilterParams), &params);

                // Dispatch compute shader
                const auto workgroupSize =
                  ShaderProgram::getComputeWorkGroupSize(32, mipWidth, mipHeight);
                _prefilterShader->dispatchCompute(workgroupSize.first, workgroupSize.second, 1);

                // Memory barrier
                glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

                // Copy result to proper face and mip level of the prefilter cubemap
                glCopyImageSubData(tempTexture,
                                   GL_TEXTURE_2D,
                                   0,
                                   0,
                                   0,
                                   0,
                                   _prefilterMap,
                                   GL_TEXTURE_CUBE_MAP,
                                   mip,
                                   0,
                                   0,
                                   face,
                                   mipWidth,
                                   mipHeight,
                                   1);
            }
        }

        // Cleanup
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &tempTexture);
        glDeleteBuffers(1, &ubo);
    }

    void createCubemapTexture(
      u32& texture, i32 resolution, GLenum internalFormat, GLenum format, bool genMips = false) {
        if (resolution < 1) { return; }
        if (texture) { glDeleteTextures(1, &texture); }
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        for (u32 i = 0; i < kCubeFaces; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         internalFormat,
                         resolution,
                         resolution,
                         0,
                         format,
                         GL_FLOAT,
                         nullptr);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        if (genMips) {
            // Use trilinear filtering for smooth transitions between mipmap levels
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        } else {
            // Use bilinear filtering when not using mipmaps
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
    }

    void updateCubemapFace(
      u32 texture, i32 face, i32 mipLevel, i32 width, i32 height, const void* data) {
        if (!texture) return;
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        auto targetFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;
        glTexSubImage2D(targetFace, mipLevel, 0, 0, width, height, GL_RGB, GL_FLOAT, data);
        if (mipLevel == 0) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    std::array<FaceData, kCubeFaces> extractCubemapFaces(i32 faceSize, const f32* data) {
        std::array<FaceData, kCubeFaces> faces;

        const auto crossWidth  = faceSize * 4;
        const auto crossHeight = faceSize * 3;
        const auto channels    = 3;  // RGB

        const std::array<std::pair<i32, i32>, kCubeFaces> faceOffsets = {{
          {2 * faceSize, faceSize},  // POSITIVE_X: right face
          {0, faceSize},             // NEGATIVE_X: left face
          {faceSize, 0},             // POSITIVE_Y: top face
          {faceSize, 2 * faceSize},  // NEGATIVE_Y: bottom face
          {faceSize, faceSize},      // POSITIVE_Z: front face
          {3 * faceSize, faceSize}   // NEGATIVE_Z: back face
        }};

        for (i32 faceIdx = 0; faceIdx < kCubeFaces; ++faceIdx) {
            auto& face  = faces[faceIdx];
            face.width  = faceSize;
            face.height = faceSize;
            face.data.resize(faceSize * faceSize * channels * sizeof(f32));

            const auto& offset = faceOffsets[faceIdx];
            f32* faceData      = RCAST<f32*>(face.data.data());

            for (i32 y = 0; y < faceSize; ++y) {
                const float* srcRow =
                  data + ((offset.second + y) * crossWidth + offset.first) * channels;
                float* dstRow = faceData + y * faceSize * channels;
                std::memcpy(dstRow, srcRow, faceSize * channels * sizeof(f32));
            }
        }

        return faces;
    }

    void setupDebugCubemapFaces(Cubemaps type) {
        u32 resolution;
        u32* faces;

        switch (type) {
            case Skybox:
                resolution = kSkyboxResolution;
                faces      = _debugSkyboxFaces;
                break;
            case Irradiance:
                resolution = kIrradianceResolution;
                faces      = _debugIrradianceFaces;
                break;
            case Prefilter:
                resolution = kPrefilterResolution;
                faces      = _debugPrefilterFaces;
                break;
        }

        glGenFramebuffers(1, &_debugFBOs[type]);
        glGenTextures(6, faces);
        for (u32 faceIdx = 0; faceIdx < kCubeFaces; ++faceIdx) {
            glBindTexture(GL_TEXTURE_2D, faces[faceIdx]);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGB16F,
                         resolution,
                         resolution,
                         0,
                         GL_RGB,
                         GL_FLOAT,
                         nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
    }

    void updateDebugCubemapFaces(const u32 cubemap, const i32 resolution, Cubemaps type) {
        auto faces = _debugIrradianceFaces;

        switch (type) {
            case Skybox:
                faces = _debugSkyboxFaces;
                break;
            case Irradiance:
                faces = _debugIrradianceFaces;
                break;
            case Prefilter:
                faces = _debugPrefilterFaces;
                break;
        }

        i32 prevFramebuffer;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFramebuffer);
        i32 prevViewport[4];
        glGetIntegerv(GL_VIEWPORT, prevViewport);
        glBindFramebuffer(GL_FRAMEBUFFER, _debugFBOs[type]);
        glViewport(0, 0, resolution, resolution);
        for (u32 faceIdx = 0; faceIdx < kCubeFaces; ++faceIdx) {
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D,
                                   faces[faceIdx],
                                   0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glm::mat4 viewMatrix;
            switch (faceIdx) {
                case 0:  // POSITIVE_X
                    viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                             glm::vec3(1.0f, 0.0f, 0.0f),
                                             glm::vec3(0.0f, -1.0f, 0.0f));
                    break;
                case 1:  // NEGATIVE_X
                    viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                             glm::vec3(-1.0f, 0.0f, 0.0f),
                                             glm::vec3(0.0f, -1.0f, 0.0f));
                    break;
                case 2:  // POSITIVE_Y
                    viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                             glm::vec3(0.0f, 1.0f, 0.0f),
                                             glm::vec3(0.0f, 0.0f, 1.0f));
                    break;
                case 3:  // NEGATIVE_Y
                    viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                             glm::vec3(0.0f, -1.0f, 0.0f),
                                             glm::vec3(0.0f, 0.0f, -1.0f));
                    break;
                case 4:  // POSITIVE_Z
                    viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                             glm::vec3(0.0f, 0.0f, 1.0f),
                                             glm::vec3(0.0f, -1.0f, 0.0f));
                    break;
                case 5:  // NEGATIVE_Z
                    viewMatrix = glm::lookAt(glm::vec3(0.0f),
                                             glm::vec3(0.0f, 0.0f, -1.0f),
                                             glm::vec3(0.0f, -1.0f, 0.0f));
                    break;
            }
            // Set up projection matrix (90 degree FOV for cube face)
            glm::mat4 projMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
            // Use skybox shader and set uniforms
            _skyboxShader->use();
            _skyboxShader->setMat4("uVP", projMatrix * viewMatrix);
            // Bind the cubemap
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
            // Render the cube
            glBindVertexArray(_cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, prevFramebuffer);
        glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
    }

    void exportMaps() {
        if (!_skyboxMap || !_brdfLut || !_irradianceMap || !_prefilterMap) {
            tinyfd_messageBox(
              "IBLGen",
              "Not all maps have been generated. Please generate all maps before exporting.",
              "ok",
              "warning",
              0);
            return;
        }

        const char* title       = "Export IBL Maps";
        const char* selectedDir = tinyfd_selectFolderDialog(title, Path::currentPath().cStr());
        if (selectedDir) { auto outputDir = Path(selectedDir); }
    }
};

int main() {
    IBLGen ibl;
    ibl.run();
    return 0;
}