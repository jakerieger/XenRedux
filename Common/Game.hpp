// Author: Jake Rieger
// Created: 12/25/2024.
//

#pragma once

#include <glad.h>
#include <GLFW/glfw3.h>

#include "Clock.hpp"
#include "Context.hpp"
#include "Types.hpp"
#include "Input/InputManager.hpp"

#include <memory>

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

        virtual void loadContent()       = 0;
        virtual void unloadContent()     = 0;
        virtual void update()            = 0;
        virtual void draw()              = 0;
        virtual void configurePipeline() = 0;
        virtual void drawDebugUI() {}

        // input events
        virtual void onKeyDown(u16 key)                    = 0;
        virtual void onKeyUp(u16 key)                      = 0;
        virtual void onMouseMove(i32 x, i32 y)             = 0;
        virtual void onMouseDown(u16 button, i32 x, i32 y) = 0;
        virtual void onMouseUp(u16 button, i32 x, i32 y)   = 0;

        void run();

        [[nodiscard]] Context* getContext() const;
        Input::InputManager& getInputManager();

    protected:
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
    };
}  // namespace x
