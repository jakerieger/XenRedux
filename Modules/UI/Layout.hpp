// Author: Jake Rieger
// Created: 1/3/2025.
//

#pragma once

#include "UI.hpp"

namespace x::UI {
    class Layout {
    public:
        virtual ~Layout()                      = default;
        virtual void apply(UIElement& element) = 0;
    };
}  // namespace x::UI