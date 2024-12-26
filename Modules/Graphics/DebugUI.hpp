// Author: Jake Rieger
// Created: 12/25/2024.
//

#pragma once

#include <GLFW/glfw3.h>

namespace x::Graphics {
    class DebugUI {
    public:
        static void init(GLFWwindow** window);
        static void beginFrame();
        static void endFrame();
        static void shutdown();

    private:
        DebugUI() = default;
    };
}  // namespace x::Graphics