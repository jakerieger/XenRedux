// Author: Jake Rieger
// Created: 12/24/2024.
//

#include "BlinnPhongMaterial.hpp"

#include "PerspectiveCamera.hpp"

namespace x {
    void BlinnPhongMaterial::apply(const std::weak_ptr<ICamera>& camera) {
        _shaderProgram->use();
        setUniform("uMaterial.ambient", _ambient);
        setUniform("uMaterial.diffuse", _diffuse);
        setUniform("uMaterial.specular", _specular);
        setUniform("uMaterial.shininess", _shininess);
        setUniform("uMaterial.shininess", _shininess);

        const auto cameraPtr = camera.lock();
        if (cameraPtr) {
            const auto perspective = DCAST<PerspectiveCamera*>(cameraPtr.get());
            if (perspective) {
                const auto viewPos = perspective->getPosition();
                setUniform("uViewPosition", viewPos);
            }
        }
    }
}  // namespace x