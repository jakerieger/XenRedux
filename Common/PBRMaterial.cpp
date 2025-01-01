// Author: Jake Rieger
// Created: 12/25/2024.
//

#include "PBRMaterial.hpp"

namespace x {
    void PBRMaterial::apply(const TransformMatrices& matrices) {
        _shaderProgram->use();

        setUniform("M", matrices.model);
        setUniform("V", matrices.view);
        setUniform("P", matrices.projection);
        setUniform("MV", matrices.modelView);
        setUniform("VP", matrices.viewProjection);
        setUniform("MVP", matrices.modelViewProjection);

        setUniform("uMaterial.albedo", _albedo);
        setUniform("uMaterial.metallic", _metallic);
        setUniform("uMaterial.roughness", _roughness);
        setUniform("uMaterial.ao", _ao);
    }

    void PBRMaterial::setAlbedo(const glm::vec3& albedo) {
        _albedo = albedo;
    }

    void PBRMaterial::setMetallic(f32 metallic) {
        _metallic = metallic;
    }

    void PBRMaterial::setRoughness(f32 roughness) {
        _roughness = roughness;
    }

    void PBRMaterial::setAO(f32 ao) {
        _ao = ao;
    }

    glm::vec3 PBRMaterial::getAlbedo() const {
        return _albedo;
    }

    f32 PBRMaterial::getMetallic() const {
        return _metallic;
    }

    f32 PBRMaterial::getRoughness() const {
        return _roughness;
    }

    f32 PBRMaterial::getAO() const {
        return _ao;
    }
}  // namespace x