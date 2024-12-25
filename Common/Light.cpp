// Author: Jake Rieger
// Created: 12/24/2024.
//

#include "Light.hpp"

namespace x {
    ILight::ILight(const glm::vec3& color, f32 intensity, bool castShadow)
        : _color(color), _intensity(intensity), _castsShadow(castShadow) {}

    void ILight::setColor(const glm::vec3& color) {
        _color = color;
    }

    void ILight::setIntensity(f32 intensity) {
        _intensity = intensity;
    }

    void ILight::setCastShadow(bool castShadow) {
        _castsShadow = castShadow;
    }

    const glm::vec3& ILight::getColor() const {
        return _color;
    }

    f32 ILight::getIntensity() const {
        return _intensity;
    }

    bool ILight::getCastShadow() const {
        return _castsShadow;
    }
}  // namespace x