// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Volatile.hpp"

#include <memory>
#include <vector>

namespace x {
    class Context {
    public:
        Context()  = default;
        ~Context() = default;

        static std::unique_ptr<Context> create() {
            return std::make_unique<Context>();
        }

        static void clear(bool depth = true);
        static void setVsync(bool vsync);

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
