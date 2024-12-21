// Author: Jake Rieger
// Created: 12/20/2024.
//

#include "Primitives.hpp"

namespace x {
    // clang-format off
    std::vector<f32> Primitives::Cube::Vertices = {
        // Positions         // Texture Coords
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // Bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // Top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // Top-left
        // Back face
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // Bottom-right
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // Top-left
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-right
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // Top-right
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, // Top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // Bottom-left
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, // Bottom-right
        // Right face
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // Top-left
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, // Top-right
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // Bottom-right
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, // Bottom-left
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // Top-left
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-right
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // Bottom-left
        // Bottom face
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // Top-right
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // Top-left
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // Bottom-left
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right
    };
    std::vector<u32> Primitives::Cube::Indices  = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        // Back face
        4, 5, 6,
        6, 7, 4,
        // Left face
        8, 9, 10,
        10, 11, 8,
        // Right face
        12, 13, 14,
        14, 15, 12,
        // Top face
        16, 17, 18,
        18, 19, 16,
        // Bottom face
        20, 21, 22,
        22, 23, 20,
    };

    std::vector<f32> Primitives::Quad::Vertices = {};
    std::vector<u32> Primitives::Quad::Indices  = {};
    // clang-format on
}  // namespace x