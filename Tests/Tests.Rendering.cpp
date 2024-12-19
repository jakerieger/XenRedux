// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "Panic.hpp"
#include "RenderSystem.hpp"
using namespace x;
using namespace x::Graphics::Commands;

#include <GLFW/glfw3.h>

int main() {
    if (!glfwInit()) { Panic("Failed to initialize GLFW"); }

    RenderSystem renderSystem;
    renderSystem.submitCommand<ClearCommand>(0.1, 0.1, 0.1, 1.f);
    renderSystem.execute();
}