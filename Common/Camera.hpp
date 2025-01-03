// Author: Jake Rieger
// Created: 12/21/2024.
//

#pragma once

#include "Types.hpp"
#include "Volatile.hpp"

#include <memory>
#include <glm/glm.hpp>

namespace x {
    class ICamera : public Volatile {
    public:
        [[nodiscard]] virtual glm::mat4 getViewProjection() const = 0;
        [[nodiscard]] virtual glm::mat4 getView() const           = 0;
        [[nodiscard]] virtual glm::mat4 getProjection() const     = 0;
        virtual void update()                                     = 0;

        template<class Camera>
        Camera* as() {
            return DCAST<Camera*>(this);
        }
    };

    class Camera {
    public:
        template<class Camera, typename... Args>
        static std::shared_ptr<Camera> create(Args&&... args) {
            return std::make_shared<Camera>(std::forward<Args>(args)...);
        }

    private:
        Camera() = default;
    };
}  // namespace x
