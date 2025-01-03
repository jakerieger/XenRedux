// Author: Jake Rieger
// Created: 1/3/2025.
//

#pragma once

#include "Types.hpp"

#include <memory>
#include <vector>
#include <unordered_map>
#include <variant>
#include <functional>
#include <glm/glm.hpp>

namespace x::UI {
    class UIElement;
    class Layout;
    class Style;

    using ElementPtr     = std::shared_ptr<UIElement>;
    using WeakElementPtr = std::weak_ptr<UIElement>;
    using PropertyValue  = std::variant<i32, f32, f64, str, glm::vec2, glm::vec4, bool>;
    using StyleMap       = std::unordered_map<std::string, PropertyValue>;
    using EventCallback  = std::function<void(const class Event&)>;

    struct Rect {
        f32 x, y, width, height;
        Rect() : x(0), y(0), width(0), height(0) {}
        Rect(f32 x, f32 y, f32 w, f32 h) : x(x), y(y), width(w), height(h) {}

        bool contains(f32 px, f32 py) const {
            return px >= x && px <= x + width && py >= y && py <= y + height;
        }
    };

    class Event {
    public:
        enum class Type {
            MouseMove,
            MousePress,
            MouseRelease,
            KeyPress,
            KeyRelease,
            Resize,
            Custom,
        };

    protected:
        Type type;
        bool handled = false;

    public:
        explicit Event(Type t) : type(t) {}
        virtual ~Event() = default;

        Type getType() const {
            return type;
        }

        bool isHandled() const {
            return handled;
        }

        void setHandled() {
            handled = true;
        }
    };
}  // namespace x::UI