// Author: Jake Rieger
// Created: 12/31/2024.
//

#include "PerspectiveCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace x {
    PerspectiveCamera::PerspectiveCamera(
      f32 fov, f32 aspect, f32 near, f32 far, glm::vec3 position, glm::vec3 lookAt, glm::vec3 up)
        : _fov(fov), _aspect(aspect), _near(near), _far(far), _position(position), _lookAt(lookAt),
          _up(up), _view(glm::mat4(1.0f)), _projection(glm::mat4(1.0f)) {
        updateViewProjection();
    }

    void PerspectiveCamera::onResize(int newWidth, int newHeight) {
        _aspect = (f32)newWidth / (f32)newHeight;
        updateViewProjection();
    }

    glm::mat4 PerspectiveCamera::getViewProjection() const {
        return _projection * _view;
    }

    glm::mat4 PerspectiveCamera::getView() const {
        return _view;
    }

    glm::mat4 PerspectiveCamera::getProjection() const {
        return _projection;
    }

    void PerspectiveCamera::update() {
        updateViewProjection();
    }

    glm::vec3 PerspectiveCamera::getPosition() const {
        return _position;
    }

    void PerspectiveCamera::updateViewProjection() {
        _projection = glm::perspective(glm::radians(_fov), _aspect, _near, _far);
        _view       = glm::lookAt(_position, _lookAt, _up);
    }
}  // namespace x