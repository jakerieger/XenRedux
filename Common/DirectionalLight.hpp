// Author: Jake Rieger
// Created: 12/24/2024.
//

#pragma once

#include "Types.hpp"
#include "Light.hpp"

namespace x {
    class DirectionalLight final : public ILight {
    public:
        explicit DirectionalLight(const glm::vec3& direction = glm::vec3(-1.0f, -1.0f, -1.0f),
                                  const glm::vec3& color     = glm::vec3(1.0f),
                                  float intensity            = 1.0f,
                                  bool castsShadows          = true)
            : ILight(color, intensity, castsShadows), _direction(glm::normalize(direction)) {}

        void setDirection(const glm::vec3& direction);
        glm::vec3 const& getDirection() const;

        void updateUniforms(const std::weak_ptr<IMaterial>& material) override;

    private:
        glm::vec3 _direction;
    };
}  // namespace x
