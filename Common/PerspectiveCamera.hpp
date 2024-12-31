// Author: Jake Rieger
// Created: 12/31/2024.
//

#pragma once

#include "Types.hpp"
#include "Camera.hpp"

namespace x {
    class PerspectiveCamera final : public ICamera {
    public:
        explicit PerspectiveCamera(f32 fov            = 45.f,
                                   f32 aspect         = 16.0f / 9.0f,
                                   f32 near           = 0.1f,
                                   f32 far            = 1000.0f,
                                   glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f),
                                   glm::vec3 lookAt   = glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f));

        void onResize(int newWidth, int newHeight) override;
        [[nodiscard]] glm::mat4 getViewProjection() const override;
        [[nodiscard]] glm::mat4 getView() const override;
        [[nodiscard]] glm::mat4 getProjection() const override;
        void update(const std::weak_ptr<Clock>& clock) override;

    private:
        f32 _fov;
        f32 _aspect;
        f32 _near;
        f32 _far;
        glm::vec3 _position;
        glm::vec3 _up;
        glm::vec3 _lookAt;
        glm::mat4 _view;
        glm::mat4 _projection;

        void updateViewProjection();
    };
}  // namespace x
