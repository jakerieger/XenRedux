// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "RenderSystem.hpp"

namespace x::Graphics {
    class ScopedBatchQueue {
    public:
        ScopedBatchQueue() : _queue(RenderSystem::requestQueue()) {}

        ~ScopedBatchQueue() {
            execute();
            _queue.reset();
        }

        template<class Command, class... Args>
        ScopedBatchQueue& submit(Args&&... args) {
            RenderSystem::submitToQueue<Command>(_queue, std::forward<Args>(args)...);
            return *this;
        }

        void execute() const {
            RenderSystem::executeQueue(_queue);
        }

    private:
        std::unique_ptr<CommandQueue> _queue;
    };
}  // namespace x::Graphics