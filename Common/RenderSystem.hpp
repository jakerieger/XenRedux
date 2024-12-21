// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Volatile.hpp"
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
            _commandQueue->execute();
        }

        static void executeQueue(const std::unique_ptr<Graphics::CommandQueue>& queue) {
            queue->execute();
        }

        static std::unique_ptr<Graphics::CommandQueue> requestQueue() {
            return std::make_unique<Graphics::CommandQueue>();
        }

        Graphics::CommandQueue& getQueue() const {
            return *_commandQueue;
        }

        // Common command shorthands
        void clear(f32 r, f32 g, f32 b, f32 a = 1.f) const {
            _commandQueue->push([&]() {
                glClearColor(r, g, b, a);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            });
        }

    private:
        std::unique_ptr<Graphics::CommandQueue> _commandQueue;
        std::vector<Volatile*> _volatiles;
    };
}  // namespace x
