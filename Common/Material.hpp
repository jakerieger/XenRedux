// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Types.hpp"
#include "Graphics/ShaderProgram.hpp"
#include "Graphics/Texture.hpp"

#include <memory>
#include <unordered_map>

namespace x {
    class Material {
    public:
        explicit Material(const std::shared_ptr<Graphics::ShaderProgram>& shader);

        void setTexture(const str& name, u32 slot, const std::weak_ptr<Graphics::Texture>& texture);
        void setUniform(const str& name, bool value);
        void setUniform(const str& name, i32 value);
        void setUniform(const str& name, f32 value);
        void setUniform(const str& name, f32 x, f32 y);
        void setUniform(const str& name, const glm::vec2& value);
        void setUniform(const str& name, f32 x, f32 y, f32 z);
        void setUniform(const str& name, const glm::vec3& value);
        void setUniform(const str& name, f32 x, f32 y, f32 z, f32 w);
        void setUniform(const str& name, const glm::vec4& value);
        void setUniform(const str& name, const glm::mat2& mat);
        void setUniform(const str& name, const glm::mat3& mat);
        void setUniform(const str& name, const glm::mat4& mat);
        void setUniform(const str& name, i32 x, i32 y);
        void setUniform(const str& name, i32 x, i32 y, i32 z);
        void setUniform(const str& name, i32 x, i32 y, i32 z, i32 w);
        void setUniform(const str& name, const f32* values, size_t count);

        void apply() const;

    private:
        std::shared_ptr<Graphics::ShaderProgram> _shaderProgram;
        std::unordered_map<i32, std::weak_ptr<Graphics::Texture>> _textures;
        std::unordered_map<str, u32> _uniforms;

        u32 getUniformLocation(const str& name);
    };
}  // namespace x