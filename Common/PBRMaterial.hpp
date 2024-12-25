// Author: Jake Rieger
// Created: 12/25/2024.
//

#pragma once

#include "Material.hpp"

namespace x {
    class PBRMaterial final : public IMaterial {
    public:
        explicit PBRMaterial(const std::shared_ptr<Graphics::ShaderProgram>& shader)
            : IMaterial(shader) {}
        void apply(const std::weak_ptr<ICamera>& camera) override;

        void setAlbedo(const glm::vec3& albedo);
        void setMetallic(f32 metallic);
        void setRoughness(f32 roughness);
        void setAO(f32 ao);

        glm::vec3 getAlbedo() const;
        f32 getMetallic() const;
        f32 getRoughness() const;
        f32 getAO() const;

    private:
        glm::vec3 _albedo = glm::vec3(1.0f);
        f32 _metallic     = 1.0f;
        f32 _roughness    = 0.0f;
        f32 _ao           = 1.0f;
    };
}  // namespace x
