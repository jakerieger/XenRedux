// Author: Jake Rieger
// Created: 12/21/2024.
//

#pragma once

#include "Camera.hpp"
#include "Clock.hpp"

namespace x {
    class PerspectiveCamera final : public ICamera {
    public:
        PerspectiveCamera(f32 fov,
                          f32 aspect,
                          f32 nearPlane,
                          f32 farPlane,
                          const glm::vec3& position,
                          const glm::vec3& lookAt,
                          const glm::vec3& up);
        glm::mat4 getViewProjection() const override;
        void update(const std::weak_ptr<Clock>& clock) override;
        void onResize(int newWidth, int newHeight) override;  // From Volatile

        void setPosition(const glm::vec3& position);
        void setLookAt(const glm::vec3& target);
        void setUp(const glm::vec3& up);
        void setPerspective(f32 fov, f32 aspect, f32 nearZ, f32 farZ);

    private:
        f32 _fov;
        f32 _aspect;
        f32 _near;
        f32 _far;
        glm::vec3 _position;
        glm::vec3 _lookAt;
        glm::vec3 _up;
        glm::mat4 _viewProjection;
        void updateViewProjection();
    };
}  // namespace x
