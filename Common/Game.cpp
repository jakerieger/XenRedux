// Author: Jake Rieger
// Created: 12/25/2024.
//

#include "Game.hpp"
#include "Panic.hpp"
#include "Graphics/DebugOpenGL.hpp"
#include "Graphics/DebugUI.hpp"

namespace x {
    static void onResize(GLFWwindow* window, const int width, const int height) {
        // Resize our volatile objects
        const auto* game = CAST<IGame*>(glfwGetWindowUserPointer(window));
        if (game) {
            auto* context = game->getContext();
            if (context) {
                for (const auto& vol : context->getVolatiles()) {
                    vol->onResize(width, height);
                }
            }
        }

        glViewport(0, 0, width, height);  // Call this last
    }

    static void onKey(GLFWwindow* window, const int key, const int, const int action, const int) {
        auto* game = CAST<IGame*>(glfwGetWindowUserPointer(window));
        if (game) {
            if (action == GLFW_PRESS) {
                game->getInputManager().updateKeyState(key, true);
            } else if (action == GLFW_RELEASE) {
                game->getInputManager().updateKeyState(key, false);
            }
        }
    }

    static void onMouse(GLFWwindow* window, const int button, const int action, const int mods) {
        auto* game = CAST<IGame*>(glfwGetWindowUserPointer(window));
        if (game) {
            if (action == GLFW_PRESS) {
                game->getInputManager().updateMouseButtonState(button, true);
            } else if (action == GLFW_RELEASE) {
                game->getInputManager().updateMouseButtonState(button, false);
            }
        }
    }

    static void onMouseMove(GLFWwindow* window, const double x, const double y) {
        auto* game = CAST<IGame*>(glfwGetWindowUserPointer(window));
        if (game) { game->getInputManager().updateMousePosition(CAST<i32>(x), CAST<i32>(y)); }
    }

    static void onMouseScroll(GLFWwindow* window, const double offsetX, const double offsetY) {
        auto* game = CAST<IGame*>(glfwGetWindowUserPointer(window));
        if (game) {}
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

        if (debug) {
            Graphics::enableDebugOutput();
            Graphics::DebugUI::init(&_window);
        }

        glViewport(0, 0, initWidth, initHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glfwSetWindowUserPointer(_window, this);  // Give callbacks access to this IGame instance.
        glfwSetFramebufferSizeCallback(_window, onResize);
        glfwSetKeyCallback(_window, onKey);
        glfwSetMouseButtonCallback(_window, onMouse);
        glfwSetCursorPosCallback(_window, onMouseMove);
        glfwSetScrollCallback(_window, onMouseScroll);

        // glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hides cursor

        glfwSwapInterval(0);  // Disabled vertical sync

        _context = Context::create();
        _clock   = Clock::create();
    }

    IGame::~IGame() {
        _clock.reset();
        _context.reset();
        if (debug) { Graphics::DebugUI::shutdown(); }
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void IGame::run() {
        loadContent();
        configurePipeline();
        _clock->start();
        glfwMaximizeWindow(_window);
        while (!glfwWindowShouldClose(_window)) {
            _clock->tick();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            {
                glfwPollEvents();
                update();
                draw();

                _clock->update();

                if (debug) {
                    Graphics::DebugUI::beginFrame();
                    drawDebugUI();
                    Graphics::DebugUI::endFrame();
                }

                glfwSwapBuffers(_window);
            }
        }
        _clock->stop();
        unloadContent();
    }

    Context* IGame::getContext() const {
        return _context.get();
    }

    Input::InputManager& IGame::getInputManager() {
        return _inputManager;
    }
}  // namespace x