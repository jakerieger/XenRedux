// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "Types.hpp"
#include "CommandQueue.hpp"

namespace x::Graphics {
    void CommandQueue::push(const std::function<void()>& command) {
        _queue.push(command);
    }

    void CommandQueue::execute() {
        while (!_queue.empty()) {
            auto& cmd = _queue.front();
            cmd();
            _queue.pop();
        }
    }

    void CommandQueue::clear() {
        while (!_queue.empty()) {
            _queue.pop();
        }
    }

    int CommandQueue::numOfCommands() const {
        return CAST<int>(_queue.size());
    }
}  // namespace x::Graphics