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
#include "Graphics/BigTriangle.hpp"
#include "Graphics/DebugOpenGL.hpp"
#include "Graphics/Pipeline.hpp"
#include "Graphics/Primitives.hpp"
#include "Graphics/RenderTarget.hpp"

using namespace x;

// TODO: Re-work the RenderSystem class
static std::shared_ptr<RenderSystem> renderSystem;
static GLFWwindow* window;
static constexpr int kWidth    = 1600;
static constexpr int kHeight   = 900;
static constexpr float kAspect = (float)kWidth / (float)kHeight;

#pragma region Shaders
#include "Graphics/Shaders/Include/BlinnPhong_VS.h"
#include "Graphics/Shaders/Include/BlinnPhong_FS.h"
#pragma endregion

#pragma region Setup
static void framebufferCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    // Update all our volatile resources
    for (const auto& v : renderSystem->getVolatiles()) {
        if (v) v->onResize(width, height);
    }
}

void initGL() {
    renderSystem = RenderSystem::create();

    if (!glfwInit()) { Panic("Failed to initialize GLFW"); }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(kWidth, kHeight, "XEN Engine", nullptr, nullptr);
    if (!window) { Panic("Failed to create GLFW window"); }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(RCAST<GLADloadproc>(glfwGetProcAddress))) {
        Panic("Failed to initialize GLAD");
    }
    Graphics::enableDebugOutput();
    glViewport(0, 0, kWidth, kHeight);
    glfwSetFramebufferSizeCallback(window, framebufferCallback);
}
#pragma endregion

int main() {
    initGL();

    // Pipeline config
    Graphics::Pipeline::setPolygonMode(false);  // for debug purposes

    // Shader testing
    const auto program = ShaderManager::createProgram(BlinnPhong_VS_Source, BlinnPhong_FS_Source);
    const auto cubeMesh =
      Mesh::create(Primitives::Cube::Vertices, Primitives::Cube::Indices, program);

    const auto camera = Camera::create<PerspectiveCamera>(45.f,
                                                          kAspect,
                                                          0.1f,
                                                          100.0f,
                                                          glm::vec3(0.0f, 0.0f, 5.0f),
                                                          glm::vec3(0.0f, 0.0f, 0.0f),
                                                          glm::vec3(0.0f, 1.0f, 0.0f));
    renderSystem->registerVolatile(camera.get());
    const auto vp = camera->getViewProjection();

    const auto renderTarget = new Graphics::RenderTarget(kWidth, kHeight, true);
    renderSystem->registerVolatile(renderTarget);

    const auto ppQuad = new Graphics::PostProcessQuad;

    const auto clock = std::make_shared<Clock>();
    clock->start();

    while (!glfwWindowShouldClose(window)) {
        clock->tick();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update stuff
        {
            cubeMesh->update(clock);
            camera->update(clock);
        }

        renderTarget->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Draw commands
        { cubeMesh->draw(camera); }
        renderTarget->unbind();

        ppQuad->draw(renderTarget->getColorTexture());

        glfwPollEvents();
        glfwSwapBuffers(window);

        clock->update();
    }

    clock->stop();

    cubeMesh->destroy();  // This shouldn't be necessary to do manually once embedded in Xen's
                          // runtime framework

    delete renderTarget;
    delete ppQuad;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}