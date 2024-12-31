// Author: Jake Rieger
// Created: 12/25/2024.
//

#include "Game.hpp"
#include "Panic.hpp"
#include "Graphics/DebugOpenGL.hpp"
#include "Graphics/DebugUI.hpp"

namespace x {
    static void resizeCallback(GLFWwindow* window, const int width, const int height) {
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

    static void
    keyCallback(GLFWwindow* window, const int key, const int, const int action, const int) {
        auto* game = CAST<IGame*>(glfwGetWindowUserPointer(window));
        if (game) {
            if (action == GLFW_PRESS) {
                game->onKeyDown(key);
            } else if (action == GLFW_RELEASE) {
                game->onKeyUp(key);
            }
        }
    }

    static void
    mouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods) {
        double curX, curY;
        glfwGetCursorPos(window, &curX, &curY);
        i32 x      = CAST<i32>(curX);
        i32 y      = CAST<i32>(curY);
        auto* game = CAST<IGame*>(glfwGetWindowUserPointer(window));
        if (game) {
            if (action == GLFW_PRESS) {
                game->onMouseDown(button, x, y);
            } else if (action == GLFW_RELEASE) {
                game->onMouseUp(button, x, y);
            }
        }
    }

    static void cursorPosCallback(GLFWwindow* window, const double x, const double y) {
        auto* game = CAST<IGame*>(glfwGetWindowUserPointer(window));
        if (game) { game->onMouseMove(CAST<i32>(x), CAST<i32>(y)); }
    }

    static void
    mouseScrollCallback(GLFWwindow* window, const double offsetX, const double offsetY) {
        auto* game = CAST<IGame*>(glfwGetWindowUserPointer(window));
        if (game) {}
    }

    static void windowCloseCallback(GLFWwindow* window) {
        auto* game = CAST<IGame*>(glfwGetWindowUserPointer(window));
        if (game) { game->quit(); }
    }

    IGame::IGame(const str& title, int initWidth, int initHeight, bool escToQuit, bool canResize)
        : title(title), initWidth(initWidth), initHeight(initHeight), width(initWidth),
          height(initHeight), escToQuit(escToQuit), _window(nullptr) {
        if (!glfwInit()) { Panic("Failed to initialize GLFW"); }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        if (!canResize) { glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); }

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
        glfwSetFramebufferSizeCallback(_window, resizeCallback);
        glfwSetWindowCloseCallback(_window, windowCloseCallback);
        // TODO: Enabling these blocks input in ImGui
        // glfwSetKeyCallback(_window, keyCallback);
        // glfwSetMouseButtonCallback(_window, mouseButtonCallback);
        // glfwSetCursorPosCallback(_window, cursorPosCallback);
        // glfwSetScrollCallback(_window, mouseScrollCallback);

        // glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hides cursor

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
        loadContent(_stateBuffer.getWriteBuffer());
        configurePipeline();
        _clock->start();

        _updateThread = std::thread(&IGame::updateLoop, this);
        renderLoop();

        _updateThread.join();
        _clock->stop();
        unloadContent();
    }

    void IGame::quit() {
        _running = false;
    }

    Context* IGame::getContext() const {
        return _context.get();
    }

    Input::InputManager& IGame::getInputManager() {
        return _inputManager;
    }

    void IGame::updateLoop() {
        while (_running) {
            auto startTime = std::chrono::high_resolution_clock::now();

            _clock->tick();

            // update game state
            auto& writeState = _stateBuffer.getWriteBuffer();
            update(writeState);

            _stateBuffer.swapWriteBuffer();
            _clock->update();

            auto endTime  = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<f32, std::milli>(endTime - startTime);
            _frameGraph.mainThreadTime.store(duration.count());
        }
    }

    void IGame::renderLoop() {
        while (_running && !glfwWindowShouldClose(_window)) {
            auto frameStart  = std::chrono::high_resolution_clock::now();
            auto renderStart = std::chrono::high_resolution_clock::now();

            u32 queryId;
            glGenQueries(1, &queryId);
            glBeginQuery(GL_TIME_ELAPSED, queryId);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glfwPollEvents();

            _stateBuffer.swapReadBuffer();
            const auto& readState = _stateBuffer.getReadBuffer();
            draw(readState);

            if (debug) {
                Graphics::DebugUI::beginFrame();
                drawDebugUI(readState);
                Graphics::DebugUI::endFrame();
            }

            glEndQuery(GL_TIME_ELAPSED);
            auto renderEnd      = std::chrono::high_resolution_clock::now();
            auto renderDuration = std::chrono::duration<f32, std::milli>(renderEnd - renderStart);
            _frameGraph.renderThreadTime.store(renderDuration.count());

            u64 gpuTime;
            glGetQueryObjectui64v(queryId, GL_QUERY_RESULT, &gpuTime);
            _frameGraph.gpuTime.store(CAST<f32>(gpuTime) /
                                      1000000.0f);  // Convert nanoseconds to milliseconds

            auto frameEnd      = std::chrono::high_resolution_clock::now();
            auto frameDuration = std::chrono::duration<f32, std::milli>(frameEnd - frameStart);
            _frameGraph.frameTime.store(frameDuration.count());

            glfwSwapBuffers(_window);
            glDeleteQueries(1, &queryId);
        }
    }
}  // namespace x