// Author: Jake Rieger
// Created: 12/24/2024.
//

#include "Model.hpp"

#include "ShaderManager.hpp"
#include "Graphics/Vertex.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Graphics/Shaders/Include/BlinnPhong_VS.h"
#include "Graphics/Shaders/Include/BlinnPhong_FS.h"

namespace x {
    Model::Model() {
        auto program = ShaderManager::createProgram(BlinnPhong_VS_Source, BlinnPhong_FS_Source);
        _material    = std::make_unique<Material>(program);
        _transform.setPosition(glm::vec3(0.0f, -2.0f, -5.0f));
    }

    bool Model::loadFromFile(const str& filename) {
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

    void Model::draw(const std::shared_ptr<ICamera>& camera) {
        _material->apply();
        _material->setUniform("uVP", camera->getViewProjection());
        _material->setUniform("uModel", _transform.getMatrix());

        for (const auto& mesh : _meshes) {
            mesh->draw();
        }
    }

    void Model::processNode(const aiNode* node, const aiScene* scene) {
        for (u32 i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            _meshes.push_back(processMesh(mesh, scene));
        }

        for (u32 i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    std::unique_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene*) const {
        std::vector<Graphics::VertexPosNormTanBiTanTex> vertices;
        std::vector<u32> indices;

        for (u32 i = 0; i < mesh->mNumVertices; i++) {
            Graphics::VertexPosNormTanBiTanTex vertex;

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