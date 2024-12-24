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
#include "Graphics/PostProcessQuad.hpp"
#include "Graphics/DebugOpenGL.hpp"
#include "Graphics/Pipeline.hpp"
#include "Graphics/Primitives.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/Effects/GaussianBlur.hpp"

using namespace x;

// TODO: Re-work the RenderSystem class
static std::shared_ptr<RenderSystem> renderSystem;
static GLFWwindow* window;
static constexpr int kWidth    = 1600;
static constexpr int kHeight   = 900;
static constexpr float kAspect = (f32)kWidth / (f32)kHeight;

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

    // Main Engine Scope
    {
        // Shader testing
        const auto program =
          ShaderManager::createProgram(BlinnPhong_VS_Source, BlinnPhong_FS_Source);
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

        // const auto renderTarget = new Graphics::RenderTarget(kWidth, kHeight, true);
        // renderSystem->registerVolatile(renderTarget);
        //
        // const auto ppQuad     = new Graphics::PostProcessQuad;
        // const auto blurEffect = new Graphics::GaussianBlurEffect;

        Graphics::RenderTarget renderTarget(kWidth, kHeight, true);
        renderSystem->registerVolatile(dynamic_cast<Volatile*>(&renderTarget));

        const Graphics::PostProcessQuad ppQuad;
        Graphics::GaussianBlurEffect blurEffect;

        blurEffect.setInputTexture(renderTarget.getColorTexture());
        blurEffect.setRenderTarget(0);  // Back-buffer since it's our final output
        blurEffect.setTextureSize(kWidth, kHeight);

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

            renderTarget.bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Draw commands
            { cubeMesh->draw(camera); }
            renderTarget.unbind();

            blurEffect.apply();
            ppQuad.draw(blurEffect.getOutputTexture());

            glfwPollEvents();
            glfwSwapBuffers(window);

            using ::std::literals::string_literals::operator""s;
            glfwSetWindowTitle(
              window,
              ("XEN Engine | FPS: "s + std::to_string(clock->getFrameRate())).c_str());

            clock->update();
        }

        clock->stop();

        cubeMesh->destroy();  // This shouldn't be necessary to do manually once embedded in Xen's
                              // runtime framework
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}