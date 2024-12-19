// Author: Jake Rieger
// Created: 12/18/2024.
//

#include "Window.hpp"
#include "WindowEvents.hpp"

#include <iostream>
using namespace x::Window;

#include "Win32/Win32Window.hpp"

int main(int argc, char* argv[]) {
    auto* window = new Win32Window("Title");

    window->registerListener<ExitEvent>(
      [](const ExitEvent& e) { std::cout << "Exit event" << std::endl; });

    window->show();
    while (!window->shouldExit()) {
        window->dispatchMessages();
    }
    delete window;
}