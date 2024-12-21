// Author: Jake Rieger
// Created: 12/19/2024.
//

#include <glad.h>
#include <GLFW/glfw3.h>

#include "Panic.hpp"
#include "RenderSystem.hpp"
#include "ShaderManager.hpp"
#include "Graphics/Pipeline.hpp"
#include "Graphics/Primitives.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/ShaderProgram.hpp"
using namespace x;
using namespace x::Graphics::Commands;

static std::shared_ptr<RenderSystem> renderSystem;
static GLFWwindow* window;

static void framebufferCallback(GLFWwindow* window, int width, int height) {
    renderSystem->submit<ViewportCommand>(0, 0, width, height);

    // Update all our volatile resources
    for (const auto& v : renderSystem->getVolatiles()) {
        if (v) v->onResize(width, height);
    }
}

#include "Graphics/Shaders/Headers/Quad_VS.h"
#include "Graphics/Shaders/Headers/Quad_FS.h"

int main() {
    renderSystem = RenderSystem::create();

    if (!glfwInit()) { Panic("Failed to initialize GLFW"); }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Tests", nullptr, nullptr);
    if (!window) { Panic("Failed to create GLFW window"); }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(RCAST<GLADloadproc>(glfwGetProcAddress))) {
        Panic("Failed to initialize GLAD");
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebufferCallback);

    // Shader testing
    const auto program = ShaderManager::createProgram(Quad_VS_Source, Quad_FS_Source);
    auto cubeVertices  = Primitives::Cube::Vertices;
    auto cubeIndices   = Primitives::Cube::Indices;

    // OpenGL setup
    u32 vao, vbo, ebo;
    const auto cubeSetupCmd = [&]() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     cubeVertices.size() * sizeof(float),
                     cubeVertices.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     cubeIndices.size() * sizeof(u32),
                     cubeIndices.data(),
                     GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              5 * sizeof(float),
                              (void*)(3 * sizeof(float)));  // Texture coords
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    };
    renderSystem->getQueue().push(cubeSetupCmd);

    // Pipeline config
    Graphics::Pipeline::setPolygonMode(true);  // for debug purposes

    while (!glfwWindowShouldClose(window)) {
        renderSystem->clear(0.f, 0.f, 0.f, 1.f);

        // Draw commands
        {
            program->use(renderSystem);
            const auto drawCmd = [&]() {
                glBindVertexArray(vao);
                glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
            };
            renderSystem->getQueue().push(drawCmd);
        }

        renderSystem->execute();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    const auto cleanupCmd = [&]() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    };
    renderSystem->getQueue().push(cleanupCmd);
    renderSystem->execute();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}