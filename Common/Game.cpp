// Author: Jake Rieger
// Created: 12/25/2024.
//

#include "Game.hpp"

#include "Panic.hpp"
#include "Graphics/DebugOpenGL.hpp"

namespace x {
    static void onResize(GLFWwindow* window, const int width, const int height) {
        // Resize our volatile objects
        const auto* game = CAST<IGame*>(glfwGetWindowUserPointer(window));
        if (game) {
            auto* renderSystem = game->getRenderSystem();
            if (renderSystem) {
                for (const auto& vol : renderSystem->getVolatiles()) {
                    vol->onResize(width, height);
                }
            }
        }

        glViewport(0, 0, width, height);  // Call this last
    }

    IGame::IGame(const str& title, int initWidth, int initHeight, bool escToQuit)
        : title(title), initWidth(initWidth), initHeight(initHeight), width(initWidth),
          height(initHeight), escToQuit(escToQuit), _window(nullptr) {
        if (!glfwInit()) { Panic("Failed to initialize GLFW"); }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        _window = glfwCreateWindow(initWidth, initHeight, title.c_str(), nullptr, nullptr);
        if (!_window) { Panic("Failed to create GLFW window"); }

        glfwMakeContextCurrent(_window);
        if (!gladLoadGLLoader(RCAST<GLADloadproc>(glfwGetProcAddress))) {
            Panic("Failed to initialize GLAD");
        }

#ifndef NDEBUG
        Graphics::enableDebugOutput();
#endif

        glViewport(0, 0, initWidth, initHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glfwSetWindowUserPointer(_window, this);  // Give callbacks access to this IGame instance.
        glfwSetFramebufferSizeCallback(_window, onResize);

        renderSystem = std::make_unique<RenderSystem>();
        clock        = std::make_shared<Clock>();
    }

    IGame::~IGame() {
        clock.reset();
        renderSystem.reset();
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void IGame::run() {
        loadContent();
        configurePipeline();
        clock->start();
        while (!glfwWindowShouldClose(_window)) {
            clock->tick();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            {
                glfwPollEvents();
                update();
                draw();
                glfwSwapBuffers(_window);
            }
            clock->update();
        }
        clock->stop();
    }

    RenderSystem* IGame::getRenderSystem() const {
        return renderSystem.get();
    }
}  // namespace x