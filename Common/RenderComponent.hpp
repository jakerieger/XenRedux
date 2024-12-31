// Author: Jake Rieger
// Created: 12/31/2024.
//

#pragma once

#include "CameraState.hpp"
#include "Types.hpp"
#include "ComponentManager.hpp"
#include "LightingState.hpp"
#include "Model.hpp"

namespace x {
    class RenderComponent {
    public:
        void draw(const CameraState& camera,
                  const LightingState& lights,
                  const x::TransformComponent& transform) const;

    private:
        x::Model _model;
        bool _castsShadows = true;
        bool _visible      = true;
    };
}  // namespace x
