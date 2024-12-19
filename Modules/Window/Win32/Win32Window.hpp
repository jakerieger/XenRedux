// Author: Jake Rieger
// Created: 12/18/2024.
//

#pragma once

#include "Window/Window.hpp"
#include "EventSystem.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace x::Window {
    class Win32Window final : public IWindow {
    public:
        explicit Win32Window(cstr title, int width = 800, int height = 600);
        ~Win32Window() override;

        void show() override;
        void hide() override;
        void minimize() override;
        void toggleMaximize() override;
        void close() override;
        void swapBuffers() override;
        void dispatchMessages() override;
        void* getNativeWindowHandle() override;
        bool shouldExit() override;
        void setShouldExit(bool exit) override;

    private:
        HWND _hwnd;
        LPCSTR _className;
        LPCSTR _title;
        MSG _msg;
        int _width, _height;
        bool _shouldExit;
        bool _maximized;

        ATOM registerClass(HINSTANCE hInstance) const;
        void initInstance(HINSTANCE hInstance);
    };
}  // namespace x::Window