// Author: Jake Rieger
// Created: 12/24/2024.
//

#pragma once

#include "Types.hpp"
#include "VertexAttribute.hpp"

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace x::Graphics {
    struct VertexPosNormTanBiTanTex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 biTangent;
        glm::vec2 texCoords;
    };

    struct VertexPosNormTex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    struct VertexPosTex {
        glm::vec3 position;
        glm::vec2 texCoords;
    };
}  // namespace x::Graphics