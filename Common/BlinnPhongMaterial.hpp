// Author: Jake Rieger
// Created: 12/24/2024.
//

#pragma once

#include "Camera.hpp"
#include "Types.hpp"
#include "Material.hpp"

namespace x {
    class BlinnPhongMaterial final : public IMaterial {
    public:
        BlinnPhongMaterial(const std::shared_ptr<Graphics::ShaderProgram>& shader,
                           const glm::vec3& ambient,
                           const glm::vec3& diffuse,
                           const glm::vec3& specular,
                           f32 shininess)
            : IMaterial(shader), _ambient(ambient), _diffuse(diffuse), _specular(specular),
              _shininess(shininess) {}
        void apply(const std::weak_ptr<ICamera>& camera) override;

    private:
        glm::vec3 _ambient;
        glm::vec3 _diffuse;
        glm::vec3 _specular;
        f32 _shininess;
    };
}  // namespace x
