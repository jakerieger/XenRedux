// Author: Jake Rieger
// Created: 12/25/2024.
//

#pragma once

#include <glad.h>
#include <GLFW/glfw3.h>

#include "Clock.hpp"
#include "Context.hpp"
#include "Types.hpp"

#include <memory>

namespace x {
    /// @brief Handles window and context creation and manages the application lifetime
    class IGame {
    public:
        explicit IGame(const str& title, int initWidth, int initHeight, bool escToQuit = false);
        virtual ~IGame();

        virtual void loadContent()       = 0;
        virtual void unloadContent()     = 0;
        virtual void update()            = 0;
        virtual void draw()              = 0;
        virtual void configurePipeline() = 0;
        virtual void drawDebugUI() {}

        void run();

        Context* getContext() const;

    protected:
        std::shared_ptr<Clock> _clock;
        std::unique_ptr<Context> _context;
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

    private:
        GLFWwindow* _window;
    };
}  // namespace x
