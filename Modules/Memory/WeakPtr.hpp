// Author: Jake Rieger
// Created: 12/17/24.
//

#pragma once

#include "SharedPtr.hpp"

namespace x::Memory {
    template<typename T>
    class WeakPtr {
    public:
        WeakPtr() : _ptr(nullptr), _cb(nullptr) {}

        WeakPtr(const SharedPtr<T>& shared) : _ptr(shared.get()), _cb(shared._cb) {
            if (_cb) { ++_cb->weakCount; }
        }

        WeakPtr(const WeakPtr& other) : _ptr(other._ptr), _cb(other._cb) {
            if (_cb) { ++_cb->weakCount; }
        }

        WeakPtr& operator=(const WeakPtr& other) {
            if (this != &other) {
                _release();
                _ptr = other._ptr;
                _cb  = other._cb;
                if (_cb) { ++_cb->weakCount; }
            }
            return *this;
        }

        WeakPtr(WeakPtr&& other) noexcept : _ptr(other._ptr), _cb(other._cb) {
            other._ptr = nullptr;
            other._cb  = nullptr;
        }

        WeakPtr& operator=(WeakPtr&& other) noexcept {
            if (this != &other) {
                _release();
                _ptr       = other._ptr;
                _cb        = other._cb;
                other._ptr = nullptr;
                other._cb  = nullptr;
            }
            return *this;
        }

        ~WeakPtr() {
            _release();
        }

        [[nodiscard]] bool expired() const {
            return !_cb || _cb->strongCount == 0;
        }

        SharedPtr<T> lock() const {
            if (expired()) return SharedPtr<T>();
            return SharedPtr<T>(_ptr, _cb);
        }

    private:
        T* _ptr;
        ControlBlock* _cb;

        void _release() {
            if (_cb) {
                --_cb->weakCount;
                if (_cb->strongCount == 0 && _cb->weakCount == 0) { delete _cb; }
            }
            _ptr = nullptr;
            _cb  = nullptr;
        }
    };
}  // namespace x::Memory