// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "Context.hpp"

#include <glad.h>

namespace x {
    void Context::clear(bool depth) {
        const auto bitmask =
          depth ? (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) : GL_COLOR_BUFFER_BIT;
        glClear(bitmask);
    }
}  // namespace x