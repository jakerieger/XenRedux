// Author: Jake Rieger
// Created: 12/24/2024.
//

#pragma once

#include "Mesh.hpp"
#include "Types.hpp"
#include "Camera.hpp"
#include "DirectionalLight.hpp"
#include "Material.hpp"
#include "Transform.hpp"

#include <vector>
#include <assimp/mesh.h>
#include <assimp/scene.h>

namespace x {
    class Model {
    public:
        Model();

        bool loadFromFile(const str& filename);
        void draw(const std::shared_ptr<ICamera>& camera,
                  DirectionalLight& sun,
                  const std::vector<std::shared_ptr<ILight>>& lights = {});

        Transform& getTransform();
        std::shared_ptr<IMaterial>& getMaterial();

        template<class T>
        T* getMaterial() {
            static_assert(std::is_base_of_v<IMaterial, T>, "T must derive from IMaterial");
            return DCAST<T*>(getMaterial().get());
        }

    private:
        std::vector<std::unique_ptr<Mesh>> _meshes;
        // TODO: Allow mapping different materials to different meshes
        std::shared_ptr<IMaterial> _material;
        Transform _transform;
        void processNode(const aiNode* node, const aiScene* scene);
        std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene) const;
    };
}  // namespace x
