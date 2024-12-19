// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "RenderSystem.hpp"

using namespace x;

int main() {
    RenderSystem renderSystem;
    renderSystem.submitCommand<Graphics::ClearCommand>(0.1, 0.1, 0.1, 1.f);
    renderSystem.executeCommandList();
}