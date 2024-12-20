// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

namespace x {
    /// @brief Trait used to signal that an object needs to be re-sized when
    /// the frame buffer size changes.
    class Volatile {
    public:
        virtual ~Volatile()                                = default;
        virtual void onResize(int newWidth, int newHeight) = 0;
    };
}  // namespace x
