// Author: Jake Rieger
// Created: 12/27/2024.
//

#pragma once

#include "Input.hpp"

#include <unordered_map>
#include <glm/vec2.hpp>

namespace x::Input {
    class InputManager {
    public:
        InputManager() = default;

        bool getKeyDown(u16 key);
        bool getKeyUp(u16 key);
        bool getMouseButtonDown(u16 button);
        bool getMouseButtonUp(u16 button);

        i32 getMouseX();
        i32 getMouseY();
        glm::vec<2, i32> getMousePosition();

        // Internal use only!
        void updateKeyState(u16 key, bool pressed);
        // Internal use only!
        void updateMouseButtonState(u16 button, bool pressed);
        // Internal use only!
        void updateMousePosition(i32 x, i32 y);

    private:
        std::unordered_map<u16, bool> _keyStates;
        std::unordered_map<u16, bool> _mouseStates;
        i32 _mouseX = 0;
        i32 _mouseY = 0;
    };
}  // namespace x::Input