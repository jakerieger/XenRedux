// Author: Jake Rieger
// Created: 12/17/24.
//

#pragma once

#include <utility>

namespace x::Memory {
    struct ControlBlock {
        size_t strongCount {1};
        size_t weakCount {1};
    };

    // Forward declare our template class for WeakPtr so it can access private members of SharedPtr
    template<typename T>
    class WeakPtr;

    template<typename T>
    class SharedPtr {
        friend class WeakPtr<T>;

    public:
        explicit SharedPtr(T* ptr = nullptr) : _ptr(ptr), _cb(ptr ? new ControlBlock() : nullptr) {}

        ~SharedPtr() {
            _release();
        }

        // Copy constructor
        SharedPtr(const SharedPtr& other) : _ptr(other._ptr), _cb(other._cb) {
            if (_cb) { ++_cb->strongCount; }
        }

        // Copy assignment
        SharedPtr& operator=(const SharedPtr& other) {
            if (this != &other) {
                _release();
                _ptr = other._ptr;
                _cb  = other._cb;
                if (_cb) { ++_cb->strongCount; }
            }
            return *this;
        }

        // Move constructor
        SharedPtr(SharedPtr&& other) noexcept : _ptr(other._ptr), _cb(other._cb) {
            other._ptr = nullptr;
            other._cb  = nullptr;
        }

        // Move assignment
        SharedPtr& operator=(SharedPtr&& other) noexcept {
            if (this != &other) {
                _release();
                _ptr       = other._ptr;
                _cb        = other._cb;
                other._ptr = nullptr;
                other._cb  = nullptr;
            }
            return *this;
        }

        T* get() const {
            return _ptr;
        }

        T& operator*() const {
            return *_ptr;
        }

        T* operator->() const {
            return _ptr;
        }

        [[nodiscard]] size_t useCount() const {
            return _cb ? _cb->strongCount : 0;
        }

    private:
        T* _ptr;
        ControlBlock* _cb;

        void _release() {
            if (_cb) {
                --_cb->strongCount;
                if (_cb->strongCount == 0) {
                    delete _ptr;
                    if (_cb->weakCount == 0) { delete _cb; }
                }
                _ptr = nullptr;
                _cb  = nullptr;
            }
        }

        SharedPtr(T* ptr, ControlBlock* cb) : _ptr(ptr), _cb(cb) {
            if (_cb) { ++_cb->strongCount; }
        }
    };

    template<typename T, typename... Args>
    SharedPtr<T> makeShared(Args&&... args) {
        return SharedPtr<T>(new T(std::forward<Args>(args)...));
    }
}  // namespace x::Memory