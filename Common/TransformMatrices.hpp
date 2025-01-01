// Author: Jake Rieger
// Created: 12/31/2024.
//

#pragma once

#include "Types.hpp"
#include <glm/glm.hpp>

namespace x {
    struct TransformMatrices {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 modelView;
        glm::mat4 viewProjection;
        glm::mat4 modelViewProjection;

        TransformMatrices(glm::mat4 m, glm::mat4 v, glm::mat4 p)
            : model(m), view(v), projection(p) {
            modelView           = v * m;
            viewProjection      = p * v;
            modelViewProjection = p * v * m;
        };
    };
}  // namespace x
