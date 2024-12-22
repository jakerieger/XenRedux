// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "Camera.hpp"
#include "Mesh.hpp"

#include <glad.h>
#include <GLFW/glfw3.h>

#include "Panic.hpp"
#include "PerspectiveCamera.hpp"
#include "RenderSystem.hpp"
#include "ShaderManager.hpp"
#include "Graphics/DebugOpenGL.hpp"
#include "Graphics/Pipeline.hpp"
#include "Graphics/Primitives.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/ShaderProgram.hpp"
#include "Memory/GpuBuffer.hpp"
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

#pragma region Shaders
#include "Graphics/Shaders/Headers/BlinnPhong_VS.h"
#include "Graphics/Shaders/Headers/BlinnPhong_FS.h"
#pragma endregion

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
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);  // Makes callbacks synchronous
    glDebugMessageCallback(Graphics::glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebufferCallback);

    // Shader testing
    const auto program = ShaderManager::createProgram(BlinnPhong_VS_Source, BlinnPhong_FS_Source);
    const auto cubeVertices = Primitives::Cube::Vertices;
    const auto cubeIndices  = Primitives::Cube::Indices;
    const auto cubeMesh     = std::make_shared<Mesh>(cubeVertices, cubeIndices, program);

    // Pipeline config
    Graphics::Pipeline::setPolygonMode(false);  // for debug purposes

    const auto camera = Camera::create<PerspectiveCamera>(45.f,
                                                          800.f / 600.f,
                                                          0.1f,
                                                          100.0f,
                                                          glm::vec3(0.0f, 0.0f, 5.0f),
                                                          glm::vec3(0.0f, 0.0f, 0.0f),
                                                          glm::vec3(0.0f, 1.0f, 0.0f));
    renderSystem->registerVolatile(camera.get());
    const auto vp = camera->getViewProjection();

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw commands
        { cubeMesh->draw(camera); }

        // camera->update(0.016f);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}