// Author: Jake Rieger
// Created: 12/21/2024.
//

#pragma once

#include "Camera.hpp"
#include "Clock.hpp"
#include <glm/gtc/quaternion.hpp>

namespace x {
    class ArcballCamera final : public ICamera {
    public:
        ArcballCamera(f32 fov,
                      f32 aspect,
                      f32 nearPlane,
                      f32 farPlane,
                      i32 viewportWidth,
                      i32 viewportHeight,
                      const glm::vec3& position,
                      const glm::vec3& lookAt,
                      const glm::vec3& up);
        [[nodiscard]] glm::mat4 getViewProjection() const override;
        void update(const std::weak_ptr<Clock>& clock) override;
        void onResize(int newWidth, int newHeight) override;  // From Volatile

        void beginDrag(i32 x, i32 y);
        void updateMousePos(i32 x, i32 y);
        void endDrag();

        void setPosition(const glm::vec3& position);
        void setLookAt(const glm::vec3& target);
        void setUp(const glm::vec3& up);
        void setPerspective(f32 fov, f32 aspect, f32 nearZ, f32 farZ);

        [[nodiscard]] glm::vec3 getPosition() const;
        [[nodiscard]] bool isDragging() const;

    private:
        f32 _fov;
        f32 _aspect;
        f32 _near;
        f32 _far;
        glm::vec3 _position;
        glm::vec3 _lookAt;
        glm::vec3 _up;
        glm::mat4 _viewProjection;
        glm::vec2 _lastMousePos = {0.f, 0.f};
        glm::vec2 _curMousePos  = {0.f, 0.f};
        bool _dragging          = false;
        f32 _radius;
        i32 _viewportWidth;
        i32 _viewportHeight;

        void updateViewProjection();
    };
}  // namespace x
