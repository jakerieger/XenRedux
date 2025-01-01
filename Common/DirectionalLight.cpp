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

    void DirectionalLight::setColor(float r, float g, float b) {
        _color = glm::vec3(r, g, b);
    }

    glm::vec3 const& DirectionalLight::getColor() const {
        return _color;
    }

    void DirectionalLight::setIntensity(f32 intensity) {
        _intensity = intensity;
    }

    f32 DirectionalLight::getIntensity() const {
        return _intensity;
    }

    void DirectionalLight::setCastsShadows(bool castsShadows) {
        _castsShadows = castsShadows;
    }

    bool DirectionalLight::getCastsShadows() const {
        return _castsShadows;
    }

    void DirectionalLight::updateUniforms(const std::weak_ptr<IMaterial>& material) const {
        const auto materialPtr = material.lock();
        if (materialPtr) {
            materialPtr->setUniform("uSun.direction", _direction);
            materialPtr->setUniform("uSun.color", _color);
            materialPtr->setUniform("uSun.intensity", _intensity);
        }
    }
}  // namespace x