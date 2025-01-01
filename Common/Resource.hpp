// Author: Jake Rieger
// Created: 12/31/2024.
//

#pragma once

namespace x {
    /// @brief Trait signaling to Xen that this class holds OpenGL resources that need to be
    /// properly cleaned up.
    struct Resource {
        virtual void release() = 0;
        virtual ~Resource()    = default;
    };
}  // namespace x
