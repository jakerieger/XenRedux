// Author: Jake Rieger
// Created: 12/17/24.
//

#pragma once
#include <utility>

namespace x::Memory {
    template<typename T>
    class UniquePtr {
    public:
        explicit UniquePtr(T* ptr = nullptr) : _ptr(ptr) {}

        ~UniquePtr() {
            delete _ptr;
        }

        UniquePtr(const UniquePtr&& other) noexcept : _ptr(other._ptr) {
            other._ptr = nullptr;
        }

        UniquePtr& operator=(UniquePtr&& other) noexcept {
            if (this != &other) {
                delete _ptr;
                _ptr       = other._ptr;
                other._ptr = nullptr;
            }
            return *this;
        }

        // Disable copy
        UniquePtr(const UniquePtr&)            = delete;
        UniquePtr& operator=(const UniquePtr&) = delete;

        T* get() const {
            return _ptr;
        }

        T& operator*() const {
            return *_ptr;
        }

        T* operator->() const {
            return _ptr;
        }

        T** release() {
            T* tmp = _ptr;
            _ptr   = nullptr;
            return tmp;
        }

        void reset(T* ptr = nullptr) {
            delete _ptr;
            _ptr = ptr;
        }

    private:
        T* _ptr;
    };

    template<typename T, typename... Args>
    UniquePtr<T> makeUnique(Args&&... args) {
        return UniquePtr<T>(new T(std::forward<Args>(args)...));
    }
}  // namespace x::Memory
// x
