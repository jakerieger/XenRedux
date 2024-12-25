// Author: Jake Rieger
// Created: 12/24/2024.
//

#include "DirectionalLight.hpp"

namespace x {
    void DirectionalLight::setDirection(const glm::vec3& direction) {
        _direction = direction;
    }

    glm::vec3 const& DirectionalLight::getDirection() const {
        return _direction;
    }

    void DirectionalLight::updateUniforms(const std::weak_ptr<IMaterial>& material) {
        const auto materialPtr = material.lock();
        if (materialPtr) {
            materialPtr->setUniform("uSun.direction", _direction);
            materialPtr->setUniform("uSun.color", _color);
            materialPtr->setUniform("uSun.intensity", _intensity);
        }
    }
}  // namespace x