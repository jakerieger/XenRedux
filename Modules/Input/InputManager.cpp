// Author: Jake Rieger
// Created: 12/27/2024.
//

#include "InputManager.hpp"

namespace x::Input {
    bool InputManager::getKeyDown(u16 key) {
        return _keyStates[key];
    }

    bool InputManager::getKeyUp(u16 key) {
        return !_keyStates[key];
    }

    bool InputManager::getMouseButtonDown(u16 button) {
        return _mouseStates[button];
    }

    bool InputManager::getMouseButtonUp(u16 button) {
        return !_mouseStates[button];
    }

    i32 InputManager::getMouseX() {
        return _mouseX;
    }

    i32 InputManager::getMouseY() {
        return _mouseY;
    }

    glm::vec<2, i32> InputManager::getMousePosition() {
        return {_mouseX, _mouseY};
    }

    void InputManager::updateKeyState(u16 key, bool pressed) {
        _keyStates[key] = pressed;
    }

    void InputManager::updateMouseButtonState(u16 button, bool pressed) {
        _mouseStates[button] = pressed;
    }

    void InputManager::updateMousePosition(i32 x, i32 y) {
        _mouseX = x;
        _mouseY = y;
    }
}  // namespace x::Input