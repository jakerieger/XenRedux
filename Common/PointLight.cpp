// Author: Jake Rieger
// Created: 12/26/2024.
//

#include "PointLight.hpp"

namespace x {
    PointLight::PointLight(const glm::vec3& position,
                           const glm::vec3& color,
                           f32 intensity,
                           f32 radius,
                           bool castsShadows,
                           f32 constant,
                           f32 linear,
                           f32 quadratic)
        : ILight(color, intensity, castsShadows), _position(position), _constant(constant),
          _linear(linear), _quadratic(quadratic), _radius(radius), _index(0) {}

    void PointLight::updateUniforms(const std::weak_ptr<IMaterial>& material) {
        const auto materialPtr = material.lock();
        if (materialPtr) {
            const auto posName       = "uPointLights[" + std::to_string(_index) + "].position";
            const auto colorName     = "uPointLights[" + std::to_string(_index) + "].color";
            const auto intensityName = "uPointLights[" + std::to_string(_index) + "].intensity";
            const auto constName     = "uPointLights[" + std::to_string(_index) + "].constant";
            const auto linearName    = "uPointLights[" + std::to_string(_index) + "].linear";
            const auto quadName      = "uPointLights[" + std::to_string(_index) + "].quadratic";
            const auto radiusName    = "uPointLights[" + std::to_string(_index) + "].radius";
            materialPtr->setUniform(posName, _position);
            materialPtr->setUniform(colorName, _color);
            materialPtr->setUniform(intensityName, _intensity);
            materialPtr->setUniform(constName, _constant);
            materialPtr->setUniform(linearName, _linear);
            materialPtr->setUniform(quadName, _quadratic);
            materialPtr->setUniform(radiusName, _radius);
        }
    }

    void PointLight::setPosition(const glm::vec3& position) {
        _position = position;
    }

    void PointLight::setIndex(u32 index) {
        if (index > (kMaxPointLights - 1)) {
            printf("Index out of bounds\n");
            return;
        }
        _index = index;
    }

    void PointLight::setRadius(f32 radius) {
        _radius = radius;
    }

    glm::vec3 PointLight::getPosition() const {
        return _position;
    }

    f32 PointLight::getRadius() const {
        return _radius;
    }
}  // namespace x