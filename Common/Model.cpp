// Author: Jake Rieger
// Created: 12/24/2024.
//

#include "Model.hpp"
#include "PBRMaterial.hpp"
#include "ShaderManager.hpp"
#include "Graphics/Vertex.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Graphics/Shaders/Include/PBR_FS.h"
#include "Graphics/Shaders/Include/PBR_VS.h"

namespace x {
    ModelHandle ModelHandle::loadFromFile(const str& filename) {
        ModelHandle handle;
        handle._modelData = std::make_shared<ModelData>();
        if (!handle._modelData->loadFromFile(filename)) { handle._modelData.reset(); }
        return handle;
    }

    ModelHandle ModelHandle::loadFromMemory(const std::vector<u8>& data) {
        Panic("Unimplemented");
    }

    bool ModelHandle::tryLoad(const str& filename, ModelHandle& outHandle) {
        outHandle = loadFromFile(filename);
        return outHandle.valid();
    }

    void ModelHandle::draw(const CameraState& camera,
                           const LightingState& lighting,
                           const TransformComponent& transform) const {
        if (_modelData) _modelData->draw(camera, lighting, transform);
    }

    void ModelHandle::release() {
        _modelData.reset();
    }

    std::shared_ptr<IMaterial> ModelHandle::getMaterial() const {
        return _modelData->_material;
    }

    void ModelData::draw(const CameraState& camera,
                         const LightingState& lighting,
                         const TransformComponent& transform) {
        const auto view       = camera.view;
        const auto projection = camera.projection;
        const auto model      = transform.getMatrix();
        const auto matrices   = TransformMatrices(model, view, projection);

        _material->apply(matrices);
        lighting.sun.updateUniforms(_material);

        for (const auto& mesh : _meshes) {
            mesh->draw();
        }
    }

    bool ModelData::loadFromFile(const str& filename) {
        auto program = ShaderManager::get().getShaderProgram(PBR_VS_Source, PBR_FS_Source);
        if (!program) { return false; }
        _material = std::make_shared<PBRMaterial>(program);

        Assimp::Importer importer;
        const auto* scene = importer.ReadFile(filename.c_str(),
                                              aiProcess_Triangulate | aiProcess_GenNormals |
                                                aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            return false;
        }
        processNode(scene->mRootNode, scene);

        return true;
    }

    bool ModelHandle::valid() const {
        return _modelData && _modelData->valid();
    }

    bool ModelData::valid() const {
        return !_meshes.empty();
    }

    void ModelData::processNode(const aiNode* node, const aiScene* scene) {
        for (u32 i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            _meshes.push_back(processMesh(mesh, scene));
        }

        for (u32 i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    std::unique_ptr<Mesh> ModelData::processMesh(aiMesh* mesh, const aiScene*) {
        std::vector<Graphics::VertexPosNormTanBiTanTex> vertices;
        std::vector<u32> indices;

        for (u32 i = 0; i < mesh->mNumVertices; i++) {
            Graphics::VertexPosNormTanBiTanTex vertex = {};

            vertex.position.x = mesh->mVertices[i].x;
            vertex.position.y = mesh->mVertices[i].y;
            vertex.position.z = mesh->mVertices[i].z;

            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;

            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;

            vertex.biTangent.x = mesh->mBitangents[i].x;
            vertex.biTangent.y = mesh->mBitangents[i].y;
            vertex.biTangent.z = mesh->mBitangents[i].z;

            if (mesh->mTextureCoords[0]) {
                vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
                vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
            } else {
                vertex.texCoords.x = 0;
                vertex.texCoords.y = 0;
            }

            vertices.push_back(vertex);
        }

        for (u32 i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (u32 j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return std::make_unique<Mesh>(
          Graphics::VertexAttributes::VertexPosition3_Normal3_Tangent3_BiTangent3_Tex2,
          vertices,
          indices);
    }
}  // namespace x