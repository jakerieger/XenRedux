// Author: Jake Rieger
// Created: 12/19/2024.
//

#include <glad.h>
#include <GLFW/glfw3.h>

#include "Panic.hpp"
#include "RenderSystem.hpp"
#include "Graphics/RenderTarget.hpp"
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

    // Test render target
    Graphics::RenderTarget renderTarget(renderSystem, 800, 600, false);

    while (!glfwWindowShouldClose(window)) {
        renderTarget.bind();
        renderSystem->submit<ClearCommand>(0.0, 0.2, 0.5, 1.f);
        renderTarget.unbind();
        const auto rtTex = renderTarget.getColorTexture();
        // More drawing commands here
        renderSystem->execute();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}