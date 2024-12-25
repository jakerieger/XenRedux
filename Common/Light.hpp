// Author: Jake Rieger
// Created: 12/24/2024.
//

#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>
#include "Types.hpp"

#include <memory>

namespace x {
    class ILight {
    public:
        explicit ILight(const glm::vec3& color = glm::vec3(1.0f),
                        f32 intensity          = 1.0f,
                        bool castShadow        = true);
        virtual ~ILight() = default;

        ILight(const ILight&)            = delete;
        ILight& operator=(const ILight&) = delete;
        ILight(ILight&&)                 = delete;
        ILight& operator=(ILight&&)      = delete;

        virtual void setColor(const glm::vec3& color);
        virtual void setIntensity(f32 intensity);
        virtual void setCastShadow(bool castShadow);

        virtual const glm::vec3& getColor() const;
        virtual f32 getIntensity() const;
        virtual bool getCastShadow() const;

        // Pure virtual method to update shader uniforms
        virtual void updateUniforms(const std::weak_ptr<IMaterial>& material) = 0;

    protected:
        glm::vec3 _color;
        f32 _intensity;
        bool _castsShadow;
        bool _active = true;
    };
}  // namespace x
