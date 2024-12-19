// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Graphics/CommandQueue.hpp"
#include "Graphics/Commands.hpp"

#include <memory>

namespace x {
    class RenderSystem {
    public:
        RenderSystem() : _commandQueue(std::make_unique<Graphics::CommandQueue>()) {}

        ~RenderSystem() {
            _commandQueue.reset();
        }

        template<class T, class... Args>
        void submitCommand(Args&&... args) {
            static_assert(std::is_base_of_v<Graphics::IRenderCommand, T>,
                          "T must derive from x::Graphics::IRenderCommand");

            auto cmd = std::make_shared<T>(std::forward<Args>(args)...);
            _commandQueue->push([cmd]() { cmd->execute(); });
        }

        template<class T, class... Args>
        void executeImmediately(Args&&... args) {
            static_assert(std::is_base_of_v<Graphics::IRenderCommand, T>,
                          "T must derive from x::Graphics::IRenderCommand");

            auto cmd = std::make_shared<T>(std::forward<Args>(args)...);
            cmd->execute();
        }

        void executeCommandList() const {
            _commandQueue->execute();
        }

    private:
        std::unique_ptr<Graphics::CommandQueue> _commandQueue;
    };
}  // namespace x
