// Author: Jake Rieger
// Created: 12/22/2024.
//

#pragma once

#include "Types.hpp"
#include "ComponentManager.hpp"
#include <glm/glm.hpp>

namespace x {
    class TransformComponent {
    public:
        TransformComponent();
        void setPosition(const glm::vec3& position);
        void setRotation(const glm::vec3& rotation);
        void setScale(const glm::vec3& scale);
        [[nodiscard]] glm::vec3 getPosition() const;
        [[nodiscard]] glm::vec3 getRotation() const;
        [[nodiscard]] glm::vec3 getScale() const;
        glm::mat4 getMatrix();
        void translate(const glm::vec3& translation);
        void rotate(const glm::vec3& rotation);
        void scale(const glm::vec3& scale);

    private:
        glm::vec3 _position;
        glm::vec3 _rotation;  // Stored in radians for internal calculations
        glm::vec3 _scale;
        glm::mat4 _transform;
        bool _needsUpdate;

        void updateMM();
        [[nodiscard]] glm::mat4 matrixRotation(const glm::vec3& eulerAngles) const;
        [[nodiscard]] glm::mat4 matrixTranslation(const glm::vec3& position) const;
        [[nodiscard]] glm::mat4 matrixScaling(const glm::vec3& scale) const;
    };
}  // namespace x
