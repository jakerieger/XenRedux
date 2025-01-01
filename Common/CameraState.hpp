// Author: Jake Rieger
// Created: 12/31/2024.
//

#pragma once

#include "Types.hpp"
#include <glm/glm.hpp>

namespace x {
    struct CameraState {
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 position;
    };
}  // namespace x
