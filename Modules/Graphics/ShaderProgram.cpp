// Author: Jake Rieger
// Created: 12/20/2024.
//

#include "ShaderProgram.hpp"
#include "DebugOpenGL.hpp"
#include "Panic.hpp"

namespace x::Graphics {
    ShaderProgram::ShaderProgram() {
        _id = glCreateProgram();
        CHECK_GL_ERROR();
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(_id);
    }

    void ShaderProgram::attachShader(const Shader& shader) {
        if (shader.getType() == ShaderType::Compute) _containsCompute = true;
        glAttachShader(_id, shader.getId());
    }

    void ShaderProgram::link() const {
        glLinkProgram(_id);
        checkErrors();
    }

    void ShaderProgram::use() const {
        glUseProgram(_id);
    }

    GLuint ShaderProgram::getId() const {
        return _id;
    }

    void ShaderProgram::dispatchCompute(u32 x, u32 y, u32 z) const {
        if (_containsCompute) {
            glDispatchCompute(x, y, z);
            CHECK_GL_ERROR();

            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            CHECK_GL_ERROR();
        } else {
            std::cout << "WARNING: ShaderProgram::dispatchCompute called on instance not "
                         "containing compute shader(s)."
                      << std::endl;
        }
    }

    void ShaderProgram::setBool(const str& name, bool value) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform1i(location, value);
        }
    }

    void ShaderProgram::setBool(const u32 location, bool value) const {
        setInt(location, value);
    }

    void ShaderProgram::setInt(const u32 location, i32 value) const {
        glUniform1i(location, value);
    }

    void ShaderProgram::setFloat(const u32 location, f32 value) const {
        glUniform1f(location, value);
    }

    void ShaderProgram::setVec2(const u32 location, const glm::vec2& value) const {
        glUniform2fv(location, 1, &value[0]);
    }

    void ShaderProgram::setVec2(const u32 location, f32 x, f32 y) const {
        glUniform2f(location, x, y);
    }

    void ShaderProgram::setVec3(const u32 location, const glm::vec3& value) const {
        glUniform3fv(location, 1, &value[0]);
    }

    void ShaderProgram::setVec3(const u32 location, f32 x, f32 y, f32 z) const {
        glUniform3f(location, x, y, z);
    }

    void ShaderProgram::setVec4(const u32 location, const glm::vec4& value) const {
        glUniform4fv(location, 1, &value[0]);
    }

    void ShaderProgram::setVec4(const u32 location, f32 x, f32 y, f32 z, f32 w) const {
        glUniform4f(location, x, y, z, w);
    }

    void ShaderProgram::setMat2(const u32 location, const glm::mat2& mat) const {
        glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
    }

    void ShaderProgram::setMat3(const u32 location, const glm::mat3& mat) const {
        glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
    }

    void ShaderProgram::setMat4(const u32 location, const glm::mat4& mat) const {
        glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
    }

    void ShaderProgram::setVec2i(const u32 location, i32 x, i32 y) const {
        glUniform2i(location, x, y);
    }

    void ShaderProgram::setVec3i(const u32 location, i32 x, i32 y, i32 z) const {
        glUniform3i(location, x, y, z);
    }

    void ShaderProgram::setVec4i(const u32 location, i32 x, i32 y, i32 z, i32 w) const {
        glUniform4i(location, x, y, z, w);
    }

    void ShaderProgram::setFloatArray(const u32 location, const f32* values, size_t count) const {
        glUniform1fv(location, (GLsizei)count, values);
    }

    void ShaderProgram::setInt(const str& name, int value) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform1i(location, value);
        }
    }

    void ShaderProgram::setFloat(const str& name, float value) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform1f(location, value);
        }
    }

    void ShaderProgram::setVec2(const str& name, const glm::vec2& value) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform2fv(location, 1, &value[0]);
        }
    }

    void ShaderProgram::setVec2(const str& name, float x, float y) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform2f(location, x, y);
        }
    }

    void ShaderProgram::setVec3(const str& name, const glm::vec3& value) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform3fv(location, 1, &value[0]);
        }
    }

    void ShaderProgram::setVec3(const str& name, float x, float y, float z) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform3f(location, x, y, z);
        }
    }

    void ShaderProgram::setVec4(const str& name, const glm::vec4& value) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform4fv(location, 1, &value[0]);
        }
    }

    void ShaderProgram::setVec4(const str& name, float x, float y, float z, float w) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform4f(location, x, y, z, w);
        }
    }

    void ShaderProgram::setMat2(const str& name, const glm::mat2& mat) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
        }
    }

    void ShaderProgram::setMat3(const str& name, const glm::mat3& mat) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
        }
    }

    void ShaderProgram::setMat4(const str& name, const glm::mat4& mat) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
        }
    }

    void ShaderProgram::setVec2i(const str& name, i32 x, i32 y) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform2i(location, x, y);
        }
    }

    void ShaderProgram::setVec3i(const str& name, i32 x, i32 y, i32 z) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform3i(location, x, y, z);
        }
    }

    void ShaderProgram::setVec4i(const str& name, i32 x, i32 y, i32 z, i32 w) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform4i(location, x, y, z, w);
        }
    }

    void ShaderProgram::setFloatArray(const str& name, const f32* values, size_t count) const {
        if (const auto location = getUniformLocation(name); location != -1) {
            glUniform1fv(location, CAST<GLsizei>(count), values);
        }
    }

    void ShaderProgram::checkErrors() const {
        int success;
        char infoLog[1024];
        glGetProgramiv(_id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(_id, 1024, NULL, infoLog);
            Panic(infoLog);
        }
    }

    i32 ShaderProgram::getUniformLocation(const str& name) const {
        return glGetUniformLocation(_id, name.c_str());
    }
}  // namespace x::Graphics