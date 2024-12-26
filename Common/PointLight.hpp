// Author: Jake Rieger
// Created: 12/26/2024.
//

#pragma once

#include "Light.hpp"

namespace x {
    static constexpr u32 kMaxPointLights = 100;

    class PointLight final : public ILight {
    public:
        explicit PointLight(const glm::vec3& position = glm::vec3(0.0f),
                            const glm::vec3& color    = glm::vec3(1.0f),
                            f32 intensity             = 1.0f,
                            f32 radius                = 45.f,
                            bool castsShadows         = true,
                            f32 constant              = 1.0f,
                            f32 linear                = 0.09f,
                            f32 quadratic             = 0.032f);

        void updateUniforms(const std::weak_ptr<IMaterial>& material) override;

        void setPosition(const glm::vec3& position);
        void setIndex(u32 index);
        void setRadius(f32 radius);

        glm::vec3 getPosition() const;
        f32 getRadius() const;

    private:
        glm::vec3 _position;
        // Attenuation factors
        f32 _constant;   // Constant term (usually 1.0)
        f32 _linear;     // Linear term, affects medium range falloff
        f32 _quadratic;  // Quadratic term, affects close range falloff
        f32 _radius;     // Maximum radius of influence
        u32 _index;
    };
}  // namespace x
