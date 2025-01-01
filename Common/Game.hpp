// Author: Jake Rieger
// Created: 12/25/2024.
//

#pragma once

#include <glad.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "Clock.hpp"
#include "Context.hpp"
#include "Types.hpp"
#include "Input/InputManager.hpp"
#include "ComponentManager.hpp"
#include "StateBuffer.hpp"

namespace x {
    /// @brief Handles window and context creation and manages the application lifetime
    class IGame {
    public:
        explicit IGame(const str& title,
                       int initWidth,
                       int initHeight,
                       bool escToQuit = false,
                       bool canResize = false);
        virtual ~IGame();

        virtual void loadContent(GameState& state) = 0;
        virtual void unloadContent()               = 0;
        virtual void update(GameState& state)      = 0;
        virtual void draw(const GameState& state)  = 0;
        virtual void configurePipeline()           = 0;
        virtual void drawDebugUI(const GameState& state) {}

        // input events
        virtual void onKeyDown(u16 key)                    = 0;
        virtual void onKeyUp(u16 key)                      = 0;
        virtual void onMouseMove(i32 x, i32 y)             = 0;
        virtual void onMouseDown(u16 button, i32 x, i32 y) = 0;
        virtual void onMouseUp(u16 button, i32 x, i32 y)   = 0;

        void run();
        void quit();

        [[nodiscard]] Context* getContext() const;
        Input::InputManager& getInputManager();

    protected:
        StateBuffer _stateBuffer;
        std::atomic<bool> _running {true};
        std::thread _updateThread;
        GLFWwindow* _window;
        std::shared_ptr<Clock> _clock;
        std::unique_ptr<Context> _context;
        Input::InputManager _inputManager;
        str title;
        int initWidth;
        int initHeight;
        int width;
        int height;
        bool escToQuit;
#ifndef NDEBUG
        const bool debug = true;
#else
        const bool debug = false;
#endif

        struct FrameGraph {
            std::atomic<f32> mainThreadTime   = 0.0f;  // ms
            std::atomic<f32> renderThreadTime = 0.0f;  // ms
            std::atomic<f32> gpuTime          = 0.0f;  // ms
            std::atomic<f32> frameTime        = 0.0f;  // ms
        } _frameGraph;

        void updateLoop();
        void renderLoop();
    };
}  // namespace x
