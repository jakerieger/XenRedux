// Author: Jake Rieger
// Created: 12/19/2024.
//

#include "Material.hpp"

namespace x {
    IMaterial::IMaterial(const std::shared_ptr<Graphics::ShaderProgram>& shader)
        : _shaderProgram(shader) {}

    void IMaterial::setTexture(const str& name,
                               u32 slot,
                               const std::weak_ptr<Graphics::Texture2D>& texture) {
        _textures.insert_or_assign(slot, texture);
        _shaderProgram->use();
        _shaderProgram->setInt(name, slot);
    }

    void IMaterial::setUniform(const str& name, bool value) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setBool(loc, value);
    }

    void IMaterial::setUniform(const str& name, i32 value) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setInt(loc, value);
    }

    void IMaterial::setUniform(const str& name, f32 value) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setFloat(loc, value);
    }

    void IMaterial::setUniform(const str& name, f32 x, f32 y) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setVec2(loc, x, y);
    }

    void IMaterial::setUniform(const str& name, const glm::vec2& value) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setVec2(loc, value);
    }

    void IMaterial::setUniform(const str& name, f32 x, f32 y, f32 z) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setVec3(loc, x, y, z);
    }

    void IMaterial::setUniform(const str& name, const glm::vec3& value) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setVec3(loc, value);
    }

    void IMaterial::setUniform(const str& name, f32 x, f32 y, f32 z, f32 w) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setVec4(loc, x, y, z, w);
    }

    void IMaterial::setUniform(const str& name, const glm::vec4& value) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setVec4(loc, value);
    }

    void IMaterial::setUniform(const str& name, const glm::mat2& mat) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setMat2(loc, mat);
    }

    void IMaterial::setUniform(const str& name, const glm::mat3& mat) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setMat3(loc, mat);
    }

    void IMaterial::setUniform(const str& name, const glm::mat4& mat) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setMat4(loc, mat);
    }

    void IMaterial::setUniform(const str& name, i32 x, i32 y) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setVec2i(loc, x, y);
    }

    void IMaterial::setUniform(const str& name, i32 x, i32 y, i32 z) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setVec3i(loc, x, y, z);
    }

    void IMaterial::setUniform(const str& name, i32 x, i32 y, i32 z, i32 w) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setVec4i(loc, x, y, z, w);
    }

    void IMaterial::setUniform(const str& name, const f32* values, size_t count) {
        const auto loc = getUniformLocation(name);
        _shaderProgram->setFloatArray(loc, values, count);
    }

    // void IMaterial::apply() const {
    //     _shaderProgram->use();
    //     // TODO: Figure out if there's a way to update all the uniforms from within the material
    //     // class
    //     for (const auto& [slot, texture] : _textures) {
    //         texture.lock()->bind(slot);
    //     }
    // }

    u32 IMaterial::getUniformLocation(const str& name) {
        if (_uniforms.find(name) == _uniforms.end()) {
            const u32 location = glGetUniformLocation(_shaderProgram->getId(), name.c_str());
            _uniforms[name]    = location;
        }
        return _uniforms.at(name);
    }
}  // namespace x