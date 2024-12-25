// Author: Jake Rieger
// Created: 12/25/2024.
//

#pragma once

#include "Material.hpp"

namespace x {
    class PBRMaterial final : public IMaterial {
    public:
        explicit PBRMaterial(const std::shared_ptr<Graphics::ShaderProgram>& shader)
            : IMaterial(shader) {}
        void apply(const std::weak_ptr<ICamera>& camera) override;
    };
}  // namespace x
