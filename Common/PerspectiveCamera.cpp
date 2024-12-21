// Author: Jake Rieger
// Created: 12/21/2024.
//

#include "PerspectiveCamera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace x {
    PerspectiveCamera::PerspectiveCamera(f32 fov,
                                         f32 aspect,
                                         f32 nearPlane,
                                         f32 farPlane,
                                         const glm::vec3& position,
                                         const glm::vec3& lookAt,
                                         const glm::vec3& up)
        : _fov(fov), _aspect(aspect), _near(nearPlane), _far(farPlane), _position(position),
          _lookAt(lookAt), _up(up), _viewProjection(glm::mat4(1.0f)) {
        updateViewProjection();
    }

    glm::mat4 PerspectiveCamera::getViewProjection() const {
        return _viewProjection;
    }

    void PerspectiveCamera::update(f32 dT) {
        updateViewProjection();
    }

    void PerspectiveCamera::onResize(int newWidth, int newHeight) {
        _aspect = newWidth / newHeight;
        updateViewProjection();
    }

    void PerspectiveCamera::setPosition(const glm::vec3& position) {
        _position = position;
        updateViewProjection();
    }

    void PerspectiveCamera::setLookAt(const glm::vec3& target) {
        _lookAt = target;
        updateViewProjection();
    }

    void PerspectiveCamera::setUp(const glm::vec3& up) {
        _up = up;
        updateViewProjection();
    }

    void PerspectiveCamera::setPerspective(f32 fov, f32 aspect, f32 nearZ, f32 farZ) {
        _fov    = fov;
        _aspect = aspect;
        _near   = nearZ;
        _far    = farZ;
        updateViewProjection();
    }

    void PerspectiveCamera::updateViewProjection() {
        const glm::mat4 projection = glm::perspective(glm::radians(_fov), _aspect, _near, _far);
        const glm::mat4 view       = glm::lookAt(_position, _lookAt, _up);
        _viewProjection            = projection * view;
    }
}  // namespace x