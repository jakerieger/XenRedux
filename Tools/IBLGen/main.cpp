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

#pragma region Shaders
#include <Graphics/Shaders/Include/Skybox_VS.h>
#include <Graphics/Shaders/Include/Skybox_FS.h>
#include <Graphics/Shaders/Include/Unlit_VS.h>
#include <Graphics/Shaders/Include/Unlit_FS.h>
#include <Graphics/Shaders/Include/BRDF_LUT_CS.h>
#include <Graphics/Shaders/Include/IrradianceMap_CS.h>
#pragma endregion

static constexpr i32 kWidth             = 1600;
static constexpr i32 kHeight            = 900;
static constexpr f32 kCubeVertices[108] = {
  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,
  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,
  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f,
  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f,
  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};
static constexpr f32 kQuadVertices[16]     = {-0.5f,
                                              -0.5f,
                                              0.0f,
                                              0.0f,
                                              0.5f,
                                              -0.5f,
                                              1.0f,
                                              0.0f,
                                              0.5f,
                                              0.5f,
                                              1.0f,
                                              1.0f,
                                              -0.5f,
                                              0.5f,
                                              0.0f,
                                              1.0f};
static constexpr u32 kQuadIndices[6]       = {0, 1, 2, 2, 3, 0};
static constexpr i32 kCubeFaces            = 6;
static constexpr i32 kBrdfResolution       = 512;
static constexpr i32 kIrradianceResolution = 32;
static constexpr i32 kPrefilterResolution  = 128;

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

        // Create the quad for drawing the BRDF LUT
        glGenVertexArrays(1, &_quadVAO);
        glBindVertexArray(_quadVAO);
        glGenBuffers(1, &_quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices, GL_STATIC_DRAW);
        glGenBuffers(1, &_quadIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kQuadIndices), kQuadIndices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)(2 * sizeof(f32)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        _skyboxShader =
          x::ShaderManager::get().getShaderProgram(Skybox_VS_Source, Skybox_FS_Source);
        if (!_skyboxShader) { Panic("Failed to load Skybox shader"); }
        _skyboxShader->use();
        _skyboxShader->setInt("uSkybox", 0);

        _quadShader = x::ShaderManager::get().getShaderProgram(Unlit_VS_Source, Unlit_FS_Source);
        if (!_quadShader) { Panic("Failed to load Quad shader"); }
        _quadShader->use();
        _quadShader->setInt("uTexture", 0);

        _brdfLutShader = x::ShaderManager::get().getShaderProgram(BRDF_LUT_CS_Source);
        if (!_brdfLutShader.get()) { Panic("Failed to load BRDF LUT shader"); }
        _irradianceShader = x::ShaderManager::get().getShaderProgram(IrradianceMap_CS_Source);
        if (!_irradianceShader.get()) { Panic("Failed to load Irradiance shader"); }

        createCubemapTexture(_irradianceMap, 512, false);
        createCubemapTexture(_prefilterMap, kPrefilterResolution, true);

        loadCubemap(R"(C:\Users\conta\Code\XenRedux\Tools\IBLGen\Data\test_sky.hdr)");
    }

    void unloadContent() override {
        glDeleteVertexArrays(1, &_cubeVAO);
        glDeleteVertexArrays(1, &_quadVAO);

        glDeleteBuffers(1, &_cubeVBO);
        glDeleteBuffers(1, &_quadVBO);
        glDeleteBuffers(1, &_quadIBO);

        glDeleteTextures(1, &_brdfLUT);
        glDeleteTextures(1, &_irradianceMap);
        glDeleteTextures(1, &_prefilterMap);

        _skyboxShader.reset();
    }

    void update() override {
        glm::mat4 v =
          glm::lookAt(glm::vec3(0.0, 0.0, 5.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 p =
          glm::perspective(glm::radians(45.0f), (f32)kWidth / (f32)kHeight, 0.1f, 100.0f);

        _skyboxShader->use();
        _skyboxShader->setMat4("uVP", p * glm::mat4(glm::mat3(v)));

        _quadShader->use();
        _quadShader->setMat4("uVP", p * v);
        _quadShader->setMat4("uModel", glm::mat4(1.0f));
    }

    void draw() override {
        x::Context::clear(true);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        // Draw cubemap box here
        _skyboxShader->use();
        glBindVertexArray(_cubeVAO);

        auto cubemap = _usePrefilter ? _prefilterMap : _irradianceMap;
        if (cubemap) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        }

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        _quadShader->use();
        if (_brdfLUT) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _brdfLUT);
        }
        glBindVertexArray(_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, _cubeVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadIBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void configurePipeline() override {}

    void drawDebugUI() override {
        ImGui::Begin("Settings",
                     nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::Button("Generate BRDF LUT")) { generateBRDF(); }

        if (ImGui::Button("Generate Irradiance Map")) { generateIrradiance(); }

        if (ImGui::Button("Generate Prefilter")) { generatePrefilter(); }

        if (ImGui::Button("Switch Irradiance/Prefilter")) { _usePrefilter = !_usePrefilter; }

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
    u32 _brdfLUT       = 0;
    u32 _irradianceMap = 0;
    u32 _prefilterMap  = 0;
    std::shared_ptr<x::Graphics::ShaderProgram> _skyboxShader;
    std::shared_ptr<x::Graphics::ShaderProgram> _quadShader;
    std::shared_ptr<x::Graphics::ShaderProgram> _brdfLutShader;
    std::shared_ptr<x::Graphics::ShaderProgram> _irradianceShader;
    std::shared_ptr<x::Graphics::ShaderProgram> _prefilterShader;
    bool _usePrefilter = false;

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

    void loadCubemap(const str& filename) {
        i32 width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        f32* data = stbi_loadf(filename.c_str(), &width, &height, &channels, 3);
        if (!data) { Panic("Couldn't load image"); }
        auto faceSize = width / 4;
        auto faces    = extractCubemapFaces(faceSize, data);
        for (int i = 0; i < faces.size(); i++) {
            updateCubemapFace(_irradianceMap, i, 0, faceSize, faceSize, faces[i].data.data());
        }
        stbi_image_free(data);
    }

    void generateBRDF() {
        if (_brdfLUT) { glDeleteTextures(1, &_brdfLUT); }
        glGenTextures(1, &_brdfLUT);
        glBindTexture(GL_TEXTURE_2D, _brdfLUT);
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
        glBindImageTexture(0, _brdfLUT, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG16F);
        const auto workgroupSize =
          x::Graphics::ShaderProgram::getComputeWorkGroupSize(16, kBrdfResolution, kBrdfResolution);
        _brdfLutShader->dispatchCompute(workgroupSize.first, workgroupSize.second, 1);
    }

    void generateIrradiance() {
        if (_irradianceMap) { glDeleteTextures(1, &_irradianceMap); }
        createCubemapTexture(_irradianceMap, kIrradianceResolution);
    }

    void generatePrefilter() {}

    void createCubemapTexture(u32& texture, i32 resolution, bool genMips = false) {
        if (resolution < 1) { return; }
        if (texture) { glDeleteTextures(1, &texture); }
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        for (u32 i = 0; i < kCubeFaces; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_RGB16F,
                         resolution,
                         resolution,
                         0,
                         GL_RGB,
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
};

int main() {
    IBLGen ibl;
    ibl.run();
    return 0;
}