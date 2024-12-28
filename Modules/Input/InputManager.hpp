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
        glm::vec2 getMousePosition();

        void setKeyPressed(u16 key);
        void setKeyReleased(u16 key);
        void setMouseButtonPressed(u16 button);
        void setMouseButtonReleased(u16 button);
        void updateMousePosition(i32 x, i32 y);

    private:
        struct State {
            bool pressed  = false;
            bool released = false;
        };

        std::unordered_map<u16, State> _keyStates;
        std::unordered_map<u16, State> _mouseStates;
        i32 _mouseX = 0;
        i32 _mouseY = 0;
    };
}  // namespace x::Input