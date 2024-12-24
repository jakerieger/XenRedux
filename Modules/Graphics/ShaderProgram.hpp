// Author: Jake Rieger
// Created: 12/20/2024.
//

#pragma once

#include <glad.h>

#include "Shader.hpp"
#include "Types.hpp"
#include <glm/glm.hpp>

namespace x::Graphics {
    class ShaderProgram {
    public:
        ShaderProgram();
        ~ShaderProgram();

        void attachShader(const Shader& shader);
        void link() const;
        void use() const;
        u32 getId() const;
        void dispatchCompute(u32 x, u32 y, u32 z) const;

        // TODO: Refactor these to use templates / generics

        // Common setters
        void setBool(const str& name, bool value) const;
        void setInt(const str& name, i32 value) const;
        void setFloat(const str& name, f32 value) const;
        void setVec2(const str& name, const glm::vec2& value) const;
        void setVec2(const str& name, f32 x, f32 y) const;
        void setVec3(const str& name, const glm::vec3& value) const;
        void setVec3(const str& name, f32 x, f32 y, f32 z) const;
        void setVec4(const str& name, const glm::vec4& value) const;
        void setVec4(const str& name, f32 x, f32 y, f32 z, f32 w) const;
        void setMat2(const str& name, const glm::mat2& mat) const;
        void setMat3(const str& name, const glm::mat3& mat) const;
        void setMat4(const str& name, const glm::mat4& mat) const;

        // Additional setters
        void setVec2i(const str& name, i32 x, i32 y) const;
        void setVec3i(const str& name, i32 x, i32 y, i32 z) const;
        void setVec4i(const str& name, i32 x, i32 y, i32 z, i32 w) const;
        void setFloatArray(const str& name, const f32* values, size_t count) const;

    private:
        u32 _id;
        bool _containsCompute = false;
        void checkErrors() const;
        i32 getUniformLocation(const str& name) const;
    };

    // Template implementations

    // template<typename Number>
}  // namespace x::Graphics