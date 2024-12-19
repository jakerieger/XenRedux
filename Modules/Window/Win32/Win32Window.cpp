// Author: Jake Rieger
// Created: 12/18/2024.
//

#include "Win32Window.hpp"
#include "Panic.hpp"
#include "Window/WindowEvents.hpp"

#include <windowsx.h>

namespace x::Window {
    static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    Win32Window::Win32Window(cstr title, int width, int height)
        : IWindow(), _title(title), _width(width), _height(height), _shouldExit(false),
          _maximized(false) {
        this->_className     = "Win32WindowClass";
        const auto hInstance = ::GetModuleHandle(NULL);
        if (registerClass(hInstance) <= 0) { Panic("Failed to register window class"); }
        initInstance(hInstance);
    }

    Win32Window::~Win32Window() {
        if (_hwnd) ::DestroyWindow(_hwnd);
        ::PostQuitMessage(0);
    }

    void Win32Window::show() {
        ::ShowWindow(_hwnd, SW_SHOWNA);
    }

    void Win32Window::hide() {
        ::ShowWindow(_hwnd, SW_HIDE);
    }

    void Win32Window::minimize() {
        ::ShowWindow(_hwnd, SW_MINIMIZE);
    }

    void Win32Window::toggleMaximize() {
        if (_maximized) ::ShowWindow(_hwnd, SW_MAXIMIZE);
        else ::ShowWindow(_hwnd, SW_RESTORE);
        _maximized = !_maximized;
    }

    void Win32Window::close() {
        ::DestroyWindow(_hwnd);
        _hwnd = nullptr;
    }

    void Win32Window::swapBuffers() {
        ::SwapBuffers(::GetDC(_hwnd));
    }

    void Win32Window::dispatchMessages() {
        if (::PeekMessage(&_msg, NULL, 0, 0, PM_REMOVE)) {
            ::TranslateMessage(&_msg);
            ::DispatchMessage(&_msg);
        }
    }

    void* Win32Window::getNativeWindowHandle() {
        return _hwnd;
    }

    bool Win32Window::shouldExit() {
        return _shouldExit;
    }

    void Win32Window::setShouldExit(bool exit) {
        _shouldExit = exit;
    }

    ATOM Win32Window::registerClass(HINSTANCE hInstance) const {
        WNDCLASSEXA wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.cbSize        = sizeof(WNDCLASSEXA);
        wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wc.lpfnWndProc   = WndProc;
        wc.hInstance     = hInstance;
        wc.hIcon         = ::LoadIcon(hInstance, IDI_APPLICATION);
        wc.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wc.lpszClassName = _className;
        wc.hIconSm       = wc.hIcon;
        wc.cbWndExtra    = sizeof(Win32Window);
        return RegisterClassExA(&wc);
    }

    void Win32Window::initInstance(HINSTANCE hInstance) {
        const auto srcWidth  = ::GetSystemMetrics(SM_CXSCREEN);
        const auto srcHeight = ::GetSystemMetrics(SM_CYSCREEN);
        const auto posX      = (srcWidth - _width) / 2;
        const auto posY      = (srcHeight - _height) / 2;

        _hwnd = ::CreateWindowExA(0,
                                  _className,
                                  _title,
                                  WS_OVERLAPPEDWINDOW,
                                  posX,
                                  posY,
                                  _width,
                                  _height,
                                  nullptr,
                                  nullptr,
                                  hInstance,
                                  this);
        if (_hwnd == nullptr) { Panic("Couldn't create window"); }

        ::SetWindowLongPtr(_hwnd, GWLP_USERDATA, RCAST<LONG_PTR>(this));
        ::UpdateWindow(_hwnd);
    }

    static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        const auto userData = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
        const auto window   = RCAST<Win32Window*>(userData);
        if (!window) return DefWindowProc(hWnd, message, wParam, lParam);

        switch (message) {
            case WM_CLOSE: {
                window->getEventDispatcher().dispatch(ExitEvent());
                window->setShouldExit(true);
                ::PostQuitMessage(0);
            } break;
            case WM_DESTROY: {
                ::PostQuitMessage(0);
            } break;
            case WM_SIZE: {
                const auto width  = LOWORD(lParam);
                const auto height = HIWORD(lParam);
                window->getEventDispatcher().dispatch(ResizeEvent(width, height));
            } break;
            case WM_PAINT: {
                window->getEventDispatcher().dispatch(PaintEvent());
            } break;
            case WM_LBUTTONDOWN: {
            } break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return S_OK;
    }
}  // namespace x::Window