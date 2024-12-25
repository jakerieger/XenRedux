// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

#include "Camera.hpp"
#include "Types.hpp"
#include "Graphics/ShaderProgram.hpp"
#include "Graphics/Texture.hpp"

#include <memory>
#include <unordered_map>

namespace x {
    class IMaterial {
    public:
        explicit IMaterial(const std::shared_ptr<Graphics::ShaderProgram>& shader);
        virtual ~IMaterial() = default;

        virtual void
        setTexture(const str& name, u32 slot, const std::weak_ptr<Graphics::Texture>& texture);
        virtual void setUniform(const str& name, bool value);
        virtual void setUniform(const str& name, i32 value);
        virtual void setUniform(const str& name, f32 value);
        virtual void setUniform(const str& name, f32 x, f32 y);
        virtual void setUniform(const str& name, const glm::vec2& value);
        virtual void setUniform(const str& name, f32 x, f32 y, f32 z);
        virtual void setUniform(const str& name, const glm::vec3& value);
        virtual void setUniform(const str& name, f32 x, f32 y, f32 z, f32 w);
        virtual void setUniform(const str& name, const glm::vec4& value);
        virtual void setUniform(const str& name, const glm::mat2& mat);
        virtual void setUniform(const str& name, const glm::mat3& mat);
        virtual void setUniform(const str& name, const glm::mat4& mat);
        virtual void setUniform(const str& name, i32 x, i32 y);
        virtual void setUniform(const str& name, i32 x, i32 y, i32 z);
        virtual void setUniform(const str& name, i32 x, i32 y, i32 z, i32 w);
        virtual void setUniform(const str& name, const f32* values, size_t count);

        virtual void apply(const std::weak_ptr<ICamera>& camera) = 0;

    protected:
        std::shared_ptr<Graphics::ShaderProgram> _shaderProgram;
        std::unordered_map<i32, std::weak_ptr<Graphics::Texture>> _textures;
        std::unordered_map<str, u32> _uniforms;

        u32 getUniformLocation(const str& name);
    };
}  // namespace x