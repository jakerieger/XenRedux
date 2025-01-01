// Author: Jake Rieger
// Created: 12/24/2024.
//

#pragma once

#include "Mesh.hpp"
#include "Types.hpp"
#include "Camera.hpp"
#include "CameraState.hpp"
#include "DirectionalLight.hpp"
#include "LightingState.hpp"
#include "Material.hpp"
#include "TransformComponent.hpp"

#include <vector>
#include <assimp/mesh.h>
#include <assimp/scene.h>

namespace x {
    class ModelData;    // Holds actual mesh data
    class ModelHandle;  // Copyable wrapper used by the ECS

    class ModelHandle {
    public:
        ModelHandle() = default;

        static ModelHandle loadFromFile(const str& filename);
        static ModelHandle loadFromMemory(const std::vector<u8>& data);
        [[nodiscard]] static bool tryLoad(const str& filename, ModelHandle& outHandle);

        void draw(const CameraState& camera,
                  const LightingState& lighting,
                  const TransformComponent& transform) const;

        std::shared_ptr<IMaterial> getMaterial() const;
        [[nodiscard]] bool valid() const;

    private:
        std::shared_ptr<ModelData> _modelData;
    };

    class ModelData {
        friend class ModelHandle;

    public:
        ModelData() = default;
        [[nodiscard]] bool valid() const;

    private:
        std::vector<std::unique_ptr<Mesh>> _meshes;
        std::shared_ptr<IMaterial> _material;

        void draw(const CameraState& camera,
                  const LightingState& lighting,
                  const TransformComponent& transform);
        bool loadFromFile(const str& filename);
        void processNode(const aiNode* node, const aiScene* scene);

        static std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
    };
}  // namespace x
