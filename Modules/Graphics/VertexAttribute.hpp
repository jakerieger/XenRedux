// Author: Jake Rieger
// Created: 12/24/2024.
//

#pragma once

#include <glad.h>
#include "Types.hpp"

#include <vector>

namespace x::Graphics {
    struct VertexAttribute {
        GLuint index;
        GLsizei size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        const void* offset;
    };

    namespace VertexAttributes {
        static std::vector<VertexAttribute> VertexPosition2_Tex2 = {
          {0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)0},
          {1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)(2 * sizeof(f32))},
        };

        static std::vector<VertexAttribute> VertexPosition3_Tex2 = {
          {0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0},
          {1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32))},
        };

        static std::vector<VertexAttribute> VertexPosition3_Normal3_Tex2 = {
          {0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)0},
          {1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(3 * sizeof(f32))},
          {2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(6 * sizeof(f32))},
        };

        static std::vector<VertexAttribute> VertexPosition3_Normal3_Tangent3_BiTangent3_Tex2 = {
          {0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(f32), (void*)0},
          {1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(f32), (void*)(3 * sizeof(f32))},
          {2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(f32), (void*)(6 * sizeof(f32))},
          {3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(f32), (void*)(9 * sizeof(f32))},
          {4, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(f32), (void*)(12 * sizeof(f32))},
        };
    }  // namespace VertexAttributes
}  // namespace x::Graphics