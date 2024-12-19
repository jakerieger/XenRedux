// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "RenderSystem.hpp"

using namespace x;
using namespace x::Graphics::Commands;

int main() {
    RenderSystem renderSystem;
    renderSystem.submitCommand<ClearCommand>(0.1, 0.1, 0.1, 1.f);
    renderSystem.execute();
}