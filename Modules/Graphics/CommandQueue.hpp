// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once
#include <functional>
#include <queue>

namespace x::Graphics {
    class CommandQueue {
    public:
        CommandQueue() = default;

        void push(const std::function<void()>& command);
        void execute();
        void clear();

    private:
        std::queue<std::function<void()>> _queue;
    };
}  // namespace x::Graphics