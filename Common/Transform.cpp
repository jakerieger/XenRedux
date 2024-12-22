// Author: Jake Rieger
// Created: 12/22/2024.
//

#include "Transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace x {
    static constexpr auto kIdentity = glm::mat4(1.0f);

    Transform::Transform()
        : _position(0.f, 0.f, 0.f), _rotation(0.f, 0.f, 0.f), _scale(1.f, 1.f, 1.f),
          _transform(kIdentity), _needsUpdate(true) {}

    void Transform::setPosition(const glm::vec3& position) {
        _position = position;
    }

    void Transform::setRotation(const glm::vec3& rotation) {
        _rotation = rotation;
    }

    void Transform::setScale(const glm::vec3& scale) {
        _scale = scale;
    }

    glm::vec3 Transform::getPosition() const {
        return _position;
    }

    glm::vec3 Transform::getRotation() const {
        return _rotation;
    }

    glm::vec3 Transform::getScale() const {
        return _scale;
    }

    glm::mat4 Transform::getMatrix() {
        if (_needsUpdate) { updateMM(); }
        return _transform;
    }

    void Transform::translate(const glm::vec3& translation) {
        _position += translation;
        _needsUpdate = true;
    }

    void Transform::rotate(const glm::vec3& rotation) {
        _rotation += rotation;
        _needsUpdate = true;
    }

    void Transform::scale(const glm::vec3& scale) {
        _scale *= scale;
        _needsUpdate = true;
    }

    void Transform::updateMM() {
        const auto translation = matrixTranslation(_position);
        const auto rotation    = matrixRotation(_rotation);
        const auto scale       = matrixScaling(_scale);
        _transform             = translation * rotation * scale;
        _needsUpdate           = false;
    }

    glm::mat4 Transform::matrixRotation(const glm::vec3& eulerAngles) const {
        const auto pitch = eulerAngles.x;  // Rotation around X-axis
        const auto yaw   = eulerAngles.y;  // Rotation around Y-axis
        const auto roll  = eulerAngles.z;  // Rotation around Z-axis
        const auto rotX  = glm::rotate(kIdentity, glm::radians(pitch), glm::vec3(1, 0, 0));
        const auto rotY  = glm::rotate(kIdentity, glm::radians(yaw), glm::vec3(0, 1, 0));
        const auto rotZ  = glm::rotate(kIdentity, glm::radians(roll), glm::vec3(0, 0, 1));
        return rotX * rotY * rotZ;
    }

    glm::mat4 Transform::matrixTranslation(const glm::vec3& position) const {
        return glm::translate(kIdentity, position);
    }

    glm::mat4 Transform::matrixScaling(const glm::vec3& scale) const {
        return glm::scale(kIdentity, scale);
    }
}  // namespace x