// Author: Jake Rieger
// Created: 12/20/2024.
//

#pragma once

#include <vector>
#include "Types.hpp"

namespace x::Primitives {
    class Cube {
    public:
        static std::vector<f32> Vertices;
        static std::vector<u32> Indices;

    private:
        Cube() = default;
    };

    class Quad {
    public:
        static std::vector<f32> Vertices;
        static std::vector<u32> Indices;

    private:
        Quad() = default;
    };

    class Triangle {
    public:
        static std::vector<f32> Vertices;
        static std::vector<u32> Indices;

    private:
        Triangle() = default;
    };
}  // namespace x::Primitives