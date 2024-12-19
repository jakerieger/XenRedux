// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Types.hpp"
#include "EventSystem.hpp"

namespace x::Window {
    class ExitEvent final : public IEvent {
    public:
        ExitEvent() = default;
    };

    class PaintEvent final : public IEvent {
    public:
        PaintEvent() = default;
    };

    class RestoreEvent final : public IEvent {
    public:
        RestoreEvent() = default;
    };

    class HideEvent final : public IEvent {
    public:
        HideEvent() = default;
    };

    class MinimizeEvent final : public IEvent {
    public:
        MinimizeEvent() = default;
    };

    class MaximizeEvent final : public IEvent {
    public:
        MaximizeEvent() = default;
    };

    class ResizeEvent final : public IEvent {
    public:
        ResizeEvent(const i32 w, const i32 h) : Width(w), Height(h) {}
        const i32 Width, Height;
    };

    class KeyDownEvent final : public IEvent {
    public:
        explicit KeyDownEvent(const i32 keyCode) : KeyCode(keyCode) {}
        const i32 KeyCode;
    };

    class KeyUpEvent final : public IEvent {
    public:
        explicit KeyUpEvent(const i32 keyCode) : KeyCode(keyCode) {}
        const i32 KeyCode;
    };

    class MouseButtonDownEvent final : public IEvent {
    public:
        explicit MouseButtonDownEvent(const i32 button) : Button(button) {}
        const i32 Button;
    };

    class MouseButtonUpEvent final : public IEvent {
    public:
        explicit MouseButtonUpEvent(const i32 button) : Button(button) {}
        const i32 Button;
    };

    class MouseMoveEvent final : public IEvent {
    public:
        explicit MouseMoveEvent(const i32 x, const i32 y) : X(x), Y(y) {}
        const i32 X, Y;
    };

    class MouseWheelEvent final : public IEvent {
    public:
        explicit MouseWheelEvent(const f32 delta) : Delta(delta) {}
        const f32 Delta;
    };

    class MouseEnterEvent final : public IEvent {
    public:
        MouseEnterEvent() = default;
    };

    class MouseLeaveEvent final : public IEvent {
    public:
        MouseLeaveEvent() = default;
    };

    class DragStartEvent final : public IEvent {
    public:
        DragStartEvent(const i32 x, const i32 y) : X(x), Y(y) {}
        const i32 X, Y;
    };

    class DragEndEvent final : public IEvent {
    public:
        DragEndEvent(const i32 x, const i32 y) : X(x), Y(y) {}
        const i32 X, Y;
    };
}  // namespace x::Window