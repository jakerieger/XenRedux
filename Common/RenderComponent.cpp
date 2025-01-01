// Author: Jake Rieger
// Created: 12/31/2024.
//

#include "RenderComponent.hpp"

namespace x {
    void RenderComponent::draw(const CameraState& camera,
                               const LightingState& lights,
                               const x::TransformComponent& transform) const {
        _model.draw(camera, lights, transform);
    }

    void RenderComponent::setModel(ModelHandle model) {
        _model = model;
    }

    void RenderComponent::setVisible(bool visible) {
        _visible = visible;
    }

    void RenderComponent::setCastsShadows(bool castsShadows) {
        _castsShadows = castsShadows;
    }

    void RenderComponent::release() {
        if (_model.valid()) { _model.release(); }
    }

    std::shared_ptr<IMaterial> RenderComponent::getMaterial() const {
        return _model.getMaterial();
    }
}  // namespace x