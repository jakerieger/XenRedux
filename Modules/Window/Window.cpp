// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "Window/Window.hpp"

#if defined(_WIN32) || defined(_WIN64)
    #include "Win32/Win32Window.hpp"
using WindowType = x::Window::Win32Window;
#else
    #error "Unsupported platform!"
#endif

namespace x::Window {
    std::unique_ptr<IWindow> createWindow(cstr title, int width, int height) {
        return std::make_unique<WindowType>(title, width, height);
    }
}  // namespace x::Window