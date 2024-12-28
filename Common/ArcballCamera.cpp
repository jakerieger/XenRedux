// Author: Jake Rieger
// Created: 12/21/2024.
//

#include "ArcballCamera.hpp"

#include "Panic.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace x {
    glm::vec3 mapToSphere(const glm::vec2& screenPos, i32 width, i32 height) {
        // Convert to normalized device coordinates (-1 to 1)
        f32 x = (2.0f * screenPos.x) / CAST<f32>(width) - 1.0f;
        f32 y = 1.0f - (2.0f * screenPos.y) / CAST<f32>(height);
        // Scale x for aspect ratio
        x *= CAST<f32>(width) / CAST<f32>(height);
        glm::vec3 point(x, y, 0.f);
        f32 sqrLen = glm::dot(point, point);
        // Project onto sphere surface if outside
        if (sqrLen > 1.0f) {
            point = glm::normalize(point);
        } else {
            point.z = std::sqrt(1.0f - sqrLen);
        }
        return point;
    }

    ArcballCamera::ArcballCamera(f32 fov,
                                 f32 aspect,
                                 f32 nearPlane,
                                 f32 farPlane,
                                 i32 viewportWidth,
                                 i32 viewportHeight,
                                 const glm::vec3& position,
                                 const glm::vec3& lookAt,
                                 const glm::vec3& up)
        : _fov(fov), _aspect(aspect), _near(nearPlane), _far(farPlane), _position(position),
          _lookAt(lookAt), _up(up), _view(glm::mat4(1.0f)), _projection(glm::mat4(1.0f)),
          _viewportWidth(viewportWidth), _viewportHeight(viewportHeight) {
        _radius = glm::length(_position - lookAt);
        updateViewProjection();
    }

    glm::mat4 ArcballCamera::getViewProjection() const {
        return _projection * _view;
    }

    glm::mat4 ArcballCamera::getView() const {
        return _view;
    }

    glm::mat4 ArcballCamera::getProjection() const {
        return _projection;
    }

    void ArcballCamera::update(const std::weak_ptr<Clock>& clock) {
        if (_dragging) {
            // Get mouse pos and map to sphere
            const glm::vec3 from = mapToSphere(_lastMousePos, _viewportWidth, _viewportHeight);
            const glm::vec3 to   = mapToSphere(_curMousePos, _viewportWidth, _viewportHeight);

            // Only compute rotation if the mouse has actually moved
            if (from != to) {
                // Calculate the rotation axis and angle
                const glm::vec3 rotationAxis = glm::normalize(glm::cross(from, to));
                const float dotProduct       = glm::dot(from, to);
                const float rotationAngle    = glm::acos(glm::min(1.0f, dotProduct));

                // Create rotation quaternion
                const glm::quat rotation = glm::angleAxis(rotationAngle, rotationAxis);

                // Calculate the camera's current position relative to look-at point
                glm::vec3 relativePos = _position - _lookAt;

                // Apply the rotation to the relative position
                relativePos = rotation * relativePos;

                // Update the camera position while maintaining the same distance from look-at
                _position = _lookAt + relativePos;

                // Adjust the up vector to maintain proper orientation
                _up = rotation * _up;
                _up = glm::normalize(_up);

                _lastMousePos = _curMousePos;
            }
        }

        updateViewProjection();
    }

    void ArcballCamera::onResize(int newWidth, int newHeight) {
        const auto fWidth  = CAST<f32>(newWidth);
        const auto fHeight = CAST<f32>(newHeight);
        _viewportWidth     = newWidth;
        _viewportHeight    = newHeight;
        _aspect            = fWidth / fHeight;
        updateViewProjection();
    }

    void ArcballCamera::beginDrag(i32 x, i32 y) {
        _dragging     = true;
        _lastMousePos = glm::vec2(x, y);
    }

    void ArcballCamera::endDrag() {
        _dragging = false;
    }

    void ArcballCamera::setPosition(const glm::vec3& position) {
        _position = position;
        updateViewProjection();
    }

    void ArcballCamera::setLookAt(const glm::vec3& target) {
        _lookAt = target;
        updateViewProjection();
    }

    void ArcballCamera::setUp(const glm::vec3& up) {
        _up = up;
        updateViewProjection();
    }

    void ArcballCamera::setPerspective(f32 fov, f32 aspect, f32 nearZ, f32 farZ) {
        _fov    = fov;
        _aspect = aspect;
        _near   = nearZ;
        _far    = farZ;
        updateViewProjection();
    }

    glm::vec3 ArcballCamera::getPosition() const {
        return _position;
    }

    bool ArcballCamera::isDragging() const {
        return _dragging;
    }

    void ArcballCamera::updateViewProjection() {
        _projection = glm::perspective(glm::radians(_fov), _aspect, _near, _far);
        _view       = glm::lookAt(_position, _lookAt, _up);
    }

    void ArcballCamera::updateMousePos(i32 x, i32 y) {
        _curMousePos = glm::vec2(x, y);
    }
}  // namespace x