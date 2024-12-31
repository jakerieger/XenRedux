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
static constexpr f32 kQuadVertices[16] = {-0.5f,
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
static constexpr u32 kQuadIndices[6]   = {0, 1, 2, 2, 3, 0};

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

        // Load the test skybox for now
    }

    void unloadContent() override {
        glDeleteVertexArrays(1, &_cubeVAO);
        glDeleteVertexArrays(1, &_quadVAO);
        glDeleteBuffers(1, &_cubeVBO);
        glDeleteBuffers(1, &_quadVBO);
        glDeleteBuffers(1, &_quadIBO);
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

        if (_irradianceMap) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _irradianceMap);
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
        ImGui::Begin("Settings");

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

    void loadCubemap(const str& filename) {}
};

int main() {
    IBLGen ibl;
    ibl.run();
    return 0;
}