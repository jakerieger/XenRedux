// Author: Jake Rieger
// Created: 12/27/2024.
//

#include "InputManager.hpp"

namespace x::Input {
    bool InputManager::getKeyDown(u16 key) {
        return _keyStates[key].pressed;
    }

    bool InputManager::getKeyUp(u16 key) {
        return !_keyStates[key].released;
    }

    bool InputManager::getMouseButtonDown(u16 button) {
        return _mouseStates[button].pressed;
    }

    bool InputManager::getMouseButtonUp(u16 button) {
        return !_mouseStates[button].released;
    }

    i32 InputManager::getMouseX() {
        return _mouseX;
    }

    i32 InputManager::getMouseY() {
        return _mouseY;
    }

    glm::vec2 InputManager::getMousePosition() {
        return {CAST<f32>(_mouseX), CAST<f32>(_mouseY)};
    }

    void InputManager::setKeyPressed(u16 key) {
        _keyStates[key].pressed = true;
    }

    void InputManager::setKeyReleased(u16 key) {
        _keyStates[key].pressed  = false;
        _keyStates[key].released = true;
    }

    void InputManager::setMouseButtonPressed(u16 button) {
        _mouseStates[button].pressed = true;
    }

    void InputManager::setMouseButtonReleased(u16 button) {
        _mouseStates[button].pressed  = false;
        _mouseStates[button].released = true;
    }

    void InputManager::updateMousePosition(i32 x, i32 y) {
        _mouseX = x;
        _mouseY = y;
    }
}  // namespace x::Input