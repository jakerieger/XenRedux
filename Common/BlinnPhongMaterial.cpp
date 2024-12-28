// Author: Jake Rieger
// Created: 12/24/2024.
//

#include "BlinnPhongMaterial.hpp"

#include "ArcballCamera.hpp"

namespace x {
    void BlinnPhongMaterial::apply(const std::weak_ptr<ICamera>& camera) {
        _shaderProgram->use();
        setUniform("uMaterial.ambient", _ambient);
        setUniform("uMaterial.diffuse", _diffuse);
        setUniform("uMaterial.specular", _specular);
        setUniform("uMaterial.shininess", _shininess);

        const auto cameraPtr = camera.lock();
        if (cameraPtr) {
            const auto perspective = DCAST<ArcballCamera*>(cameraPtr.get());
            if (perspective) {
                const auto viewPos = perspective->getPosition();
                setUniform("uViewPosition", viewPos);
            }
        }
    }

    void BlinnPhongMaterial::setAmbient(const glm::vec3& ambient) {
        _ambient = ambient;
    }

    void BlinnPhongMaterial::setDiffuse(const glm::vec3& diffuse) {
        _diffuse = diffuse;
    }

    void BlinnPhongMaterial::setSpecular(const glm::vec3& specular) {
        _specular = specular;
    }

    void BlinnPhongMaterial::setShininess(f32 shininess) {
        _shininess = shininess;
    }
}  // namespace x