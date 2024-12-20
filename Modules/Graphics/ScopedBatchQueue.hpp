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
        ScopedBatchQueue* submit(Args&&... args) {
            static_assert(std::is_base_of_v<Graphics::IRenderCommand, Command>,
                          "T must derive from x::Graphics::IRenderCommand");
            auto cmd = std::make_shared<Command>(std::forward<Args>(args)...);
            _queue->push([cmd]() { cmd->execute(); });
            return this;
        }

        void execute() const {
            RenderSystem::executeQueue(_queue);
        }

    private:
        std::unique_ptr<CommandQueue> _queue;
    };
}  // namespace x::Graphics