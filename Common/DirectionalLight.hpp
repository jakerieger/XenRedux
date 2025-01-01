// Author: Jake Rieger
// Created: 12/24/2024.
//

#pragma once

#include "Types.hpp"
#include "Material.hpp"

#include <glm/glm.hpp>
#include <memory>

namespace x {
    class DirectionalLight {
    public:
        explicit DirectionalLight(const glm::vec3& direction = glm::vec3(-0.5f, -0.5f, -0.5f),
                                  const glm::vec3& color     = glm::vec3(1.0f),
                                  f32 intensity              = 1.0f,
                                  bool castsShadows          = true)
            : _direction(direction), _color(color), _intensity(intensity),
              _castsShadows(castsShadows) {}

        void setDirection(const glm::vec3& direction);
        glm::vec3 const& getDirection() const;

        void setColor(float r, float g, float b);
        glm::vec3 const& getColor() const;

        void setIntensity(f32 intensity);
        f32 getIntensity() const;

        void setCastsShadows(bool castsShadows);
        bool getCastsShadows() const;

        void updateUniforms(const std::weak_ptr<IMaterial>& material) const;

    private:
        glm::vec3 _direction;
        glm::vec3 _color;
        f32 _intensity;
        bool _castsShadows;
    };
}  // namespace x
