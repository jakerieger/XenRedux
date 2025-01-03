// Author: Jake Rieger
// Created: 12/31/2024.
//

#pragma once

#include "CameraState.hpp"
#include "Types.hpp"
#include "ComponentManager.hpp"
#include "LightingState.hpp"
#include "Model.hpp"
#include "Resource.hpp"

namespace x {
    class RenderComponent : public Resource {
    public:
        RenderComponent() = default;

        RenderComponent(const RenderComponent&)            = default;
        RenderComponent& operator=(const RenderComponent&) = default;
        RenderComponent(RenderComponent&&)                 = default;
        RenderComponent& operator=(RenderComponent&&)      = default;

        void draw(const CameraState& camera,
                  const LightingState& lights,
                  const x::TransformComponent& transform) const;
        void setModel(ModelHandle model);
        void setVisible(bool visible);
        void setCastsShadows(bool castsShadows);
        void release() override;
        std::shared_ptr<IMaterial> getMaterial() const;

    private:
        x::ModelHandle _model;
        bool _castsShadows = true;
        bool _visible      = true;
    };
}  // namespace x
