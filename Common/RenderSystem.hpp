// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Graphics/CommandQueue.hpp"
#include "Graphics/Commands.hpp"

#include <memory>
#include <catch2/internal/catch_unique_ptr.hpp>

namespace x {
    class RenderSystem {
    public:
        RenderSystem() : _commandQueue(std::make_unique<Graphics::CommandQueue>()) {}

        ~RenderSystem() {
            _commandQueue.reset();
        }

        template<class T, class... Args>
        RenderSystem* submitCommand(Args&&... args) {
            static_assert(std::is_base_of_v<Graphics::IRenderCommand, T>,
                          "T must derive from x::Graphics::IRenderCommand");
            auto cmd = std::make_shared<T>(std::forward<Args>(args)...);
            _commandQueue->push([cmd]() { cmd->execute(); });
            return this;
        }

        template<class T, class... Args>
        static void submitToQueue(const std::unique_ptr<Graphics::CommandQueue>& queue,
                                  Args&&... args) {
            static_assert(std::is_base_of_v<Graphics::IRenderCommand, T>,
                          "T must derive from x::Graphics::IRenderCommand");
            auto cmd = std::make_shared<T>(std::forward<Args>(args)...);
            queue->push([cmd]() { cmd->execute(); });
        }

        template<class T, class... Args>
        RenderSystem* executeImmediately(Args&&... args) {
            static_assert(std::is_base_of_v<Graphics::IRenderCommand, T>,
                          "T must derive from x::Graphics::IRenderCommand");

            auto cmd = std::make_shared<T>(std::forward<Args>(args)...);
            cmd->execute();
            return this;
        }

        void execute() const {
            _commandQueue->execute();
        }

        static void executeQueue(const std::unique_ptr<Graphics::CommandQueue>& queue) {
            queue->execute();
        }

        static std::unique_ptr<Graphics::CommandQueue> requestBatchQueue() {
            return std::make_unique<Graphics::CommandQueue>();
        }

    private:
        std::unique_ptr<Graphics::CommandQueue> _commandQueue;
    };
}  // namespace x
