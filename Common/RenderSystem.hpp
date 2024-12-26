// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Volatile.hpp"

#include <memory>
#include <vector>

namespace x {
    class RenderSystem {
    public:
        RenderSystem()  = default;
        ~RenderSystem() = default;

        static std::unique_ptr<RenderSystem> create() {
            return std::make_unique<RenderSystem>();
        }

        static void clear(bool depth = true);

        void registerVolatile(Volatile* vol) {
            _volatiles.push_back(vol);
        }

        std::vector<Volatile*> getVolatiles() {
            return _volatiles;
        }

    private:
        std::vector<Volatile*> _volatiles;
    };
}  // namespace x
