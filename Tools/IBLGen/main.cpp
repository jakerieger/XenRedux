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
#pragma endregion

static constexpr i32 kWidth                = 1600;
static constexpr i32 kHeight               = 900;
static constexpr i32 kCubeFaces            = 6;
static constexpr i32 kBrdfResolution       = 512;  // 512x512
static constexpr i32 kIrradianceResolution = 64;   // 32x32
static constexpr i32 kPrefilterResolution  = 128;  // 128x128

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

        if (_debugFBO) { glDeleteFramebuffers(1, &_debugFBO); }
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
        generateIrradiance();

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

        if (ImGui::Button("Generate BRDF LUT")) { generateBRDF(); }

        if (ImGui::Button("Generate Irradiance Map")) { generateIrradiance(); }

        if (ImGui::Button("Generate Prefilter")) { generatePrefilter(); }

        ImGui::Separator();

        if (_skyboxMap) { ImGui::Text("Skybox"); }

        if (_brdfLut) {
            ImGui::Text("BRDF LUT");
            ImVec2 size(200.f, 200.f);
            ImGui::Image((ImTextureID)(int*)_brdfLut, size, ImVec2(0, 1), ImVec2(1, 0));
        }

        if (_irradianceMap) {
            ImGui::Text("Irradiance Map");
            if (!_debugIrradianceFaces[0]) setupDebugCubemapFaces(Cubemaps::Irradiance);
            updateDebugCubemapFaces(_irradianceMap, kIrradianceResolution);
            f32 faceSize   = 64.0f;
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
                ImGui::SameLine();
            }

            // Bottom face (-Y)
            ImGui::SetCursorPos(ImVec2(basePos.x + faceSize, basePos.y + 2 * faceSize));
            ImGui::Image((ImTextureID)(intptr_t)_debugIrradianceFaces[3],
                         ImVec2(faceSize, faceSize));
        }

        if (_prefilterMap) {
            // ImGui::Text("Prefilter Map");
            // if (!_debugPrefilterFaces[0]) setupDebugCubemapFaces(Cubemaps::Prefilter);
            // updateDebugCubemapFaces(_prefilterMap, kPrefilterResolution);
            // f32 faceSize   = 64.0f;
            // ImVec2 basePos = ImGui::GetCursorPos();
            //
            // // Draw the faces in a cross pattern
            // //     [+Y]
            // // [-X][+Z][+X][-Z]
            // //     [-Y]
            //
            // // Top face (+Y)
            // ImGui::SetCursorPos(ImVec2(basePos.x + faceSize, basePos.y));
            // ImGui::Image((ImTextureID)(intptr_t)_debugPrefilterFaces[2],
            //              ImVec2(faceSize, faceSize));
            //
            // // Middle row
            // ImGui::SetCursorPos(ImVec2(basePos.x, basePos.y + faceSize));
            // for (int i = 0; i < 4; i++) {
            //     int faceIdx = (i == 0) ? 1 : (i == 1) ? 4 : (i == 2) ? 0 : 5;  // -X, +Z, +X, -Z
            //     ImGui::Image((ImTextureID)(intptr_t)_debugPrefilterFaces[faceIdx],
            //                  ImVec2(faceSize, faceSize));
            //     ImGui::SameLine();
            // }
            //
            // // Bottom face (-Y)
            // ImGui::SetCursorPos(ImVec2(basePos.x + faceSize, basePos.y + 2 * faceSize));
            // ImGui::Image((ImTextureID)(intptr_t)_debugPrefilterFaces[3],
            //              ImVec2(faceSize, faceSize));
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
    u32 _debugFBO;
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

        // Generate mips if needed
        // glBindTexture(GL_TEXTURE_CUBE_MAP, _irradianceMap);
        // glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    void generatePrefilter() {}

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
        switch (type) {
            case Skybox:
                break;
            case Irradiance: {
                glGenFramebuffers(1, &_debugFBO);
                glGenTextures(6, _debugIrradianceFaces);
                for (u32 faceIdx = 0; faceIdx < kCubeFaces; ++faceIdx) {
                    glBindTexture(GL_TEXTURE_2D, _debugIrradianceFaces[faceIdx]);
                    glTexImage2D(GL_TEXTURE_2D,
                                 0,
                                 GL_RGB16F,
                                 kIrradianceResolution,
                                 kIrradianceResolution,
                                 0,
                                 GL_RGB,
                                 GL_FLOAT,
                                 nullptr);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                }
            } break;
            case Prefilter:
                break;
        }
    }

    void updateDebugCubemapFaces(const u32 cubemap, const i32 resolution) {
        glBindFramebuffer(GL_FRAMEBUFFER, _debugFBO);
        i32 viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glViewport(0, 0, resolution, resolution);
        for (u32 faceIdx = 0; faceIdx < kCubeFaces; ++faceIdx) {
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D,
                                   _debugIrradianceFaces[faceIdx],
                                   0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, resolution, resolution);
        }
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

int main() {
    IBLGen ibl;
    ibl.run();
    return 0;
}