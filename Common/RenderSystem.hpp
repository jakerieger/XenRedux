// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Volatile.hpp"
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

        static std::shared_ptr<RenderSystem> create() {
            return std::make_shared<RenderSystem>();
        }

        void registerVolatile(Volatile* vol) {
            _volatiles.push_back(vol);
        }

        std::vector<Volatile*> getVolatiles() {
            return _volatiles;
        }

        template<class T, class... Args>
        RenderSystem* submit(Args&&... args) {
            static_assert(std::is_base_of_v<Graphics::IRenderCommand, T>,
                          "T must derive from x::Graphics::IRenderCommand");
            auto cmd = std::make_shared<T>(std::forward<Args>(args)...);
            _commandQueue->push([cmd]() { cmd->execute(); });
            return this;
        }

        template<class T, class... Args>
        RenderSystem* executeImmediate(Args&&... args) {
            static_assert(std::is_base_of_v<Graphics::IRenderCommand, T>,
                          "T must derive from x::Graphics::IRenderCommand");

            auto cmd = std::make_shared<T>(std::forward<Args>(args)...);
            cmd->execute();
            return this;
        }

        void execute() const {
            // printf("Executing command queue with (%d) command(s)...\n",
            //        _commandQueue->numOfCommands());
            _commandQueue->execute();
        }

        static void executeQueue(const std::unique_ptr<Graphics::CommandQueue>& queue) {
            queue->execute();
        }

        static std::unique_ptr<Graphics::CommandQueue> requestQueue() {
            return std::make_unique<Graphics::CommandQueue>();
        }

    private:
        std::unique_ptr<Graphics::CommandQueue> _commandQueue;
        std::vector<Volatile*> _volatiles;
    };
}  // namespace x
