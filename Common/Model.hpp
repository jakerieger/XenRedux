// Author: Jake Rieger
// Created: 12/24/2024.
//

#pragma once

#include "Mesh.hpp"
#include "Types.hpp"
#include "Camera.hpp"
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
        void draw(const std::shared_ptr<ICamera>& camera);

        Transform& getTransform();

    private:
        std::vector<std::unique_ptr<Mesh>> _meshes;
        // TODO: Allow mapping different materials to different meshes
        std::unique_ptr<Material> _material;
        Transform _transform;
        void processNode(const aiNode* node, const aiScene* scene);
        std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene) const;
    };
}  // namespace x
