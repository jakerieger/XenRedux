// Author: Jake Rieger
// Created: 12/19/2024.
//

#include <glad.h>
#include <GLFW/glfw3.h>

#include "Panic.hpp"
#include "RenderSystem.hpp"
using namespace x;
using namespace x::Graphics::Commands;

static RenderSystem renderSystem;
static GLFWwindow* window;

static void framebufferCallback(GLFWwindow* window, int width, int height) {
    renderSystem.submitCommand<ViewportCommand>(0, 0, width, height);
}

int main() {
    if (!glfwInit()) { Panic("Failed to initialize GLFW"); }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Tests", NULL, NULL);
    if (!window) { Panic("Failed to create GLFW window"); }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { Panic("Failed to initialize GLAD"); }
    glViewport(0, 0, 640, 480);
    glfwSetFramebufferSizeCallback(window, framebufferCallback);

    while (!glfwWindowShouldClose(window)) {
        renderSystem.submitCommand<ClearCommand>(0.1, 0.1, 0.1, 1.f);
        // More drawing commands here
        renderSystem.execute();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}