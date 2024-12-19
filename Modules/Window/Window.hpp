// Author: Jake Rieger
// Created: 12/18/2024.
//

#pragma once

#include "Types.hpp"
#include "EventSystem.hpp"

namespace x::Window {
    class IWindow {
    public:
        IWindow() : _eventDispatcher(std::make_shared<EventDispatcher>()) {}
        virtual ~IWindow() = default;

        virtual void show()                   = 0;
        virtual void hide()                   = 0;
        virtual void minimize()               = 0;
        virtual void toggleMaximize()         = 0;
        virtual void close()                  = 0;
        virtual void swapBuffers()            = 0;
        virtual void dispatchMessages()       = 0;
        virtual void* getNativeWindowHandle() = 0;
        virtual bool shouldExit()             = 0;
        virtual void setShouldExit(bool exit) = 0;

        template<typename EventType>
        void registerListener(std::function<void(const EventType&)> callback) {
            _eventDispatcher->registerListener(callback);
        }

        EventDispatcher& getEventDispatcher() const {
            return *_eventDispatcher;
        }

        friend class Win32Window;
        friend class X11Window;
        friend class WaylandWindow;
        friend class NSWindow;

    protected:
        std::shared_ptr<EventDispatcher> _eventDispatcher;
    };

    std::unique_ptr<IWindow> createWindow(cstr title, int width, int height);
}  // namespace x::Window