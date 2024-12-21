// Author: Jake Rieger
// Created: 12/20/2024.
//

#include "ShaderProgram.hpp"
#include "Panic.hpp"

namespace x::Graphics {
    ShaderProgram::ShaderProgram() {
        _id = glCreateProgram();
        assert(_id != 0);
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(_id);
    }

    void ShaderProgram::attachShader(const Shader& shader) const {
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

    void ShaderProgram::setBool(const str& name, bool value) const {
        glUniform1i(glGetUniformLocation(_id, name.c_str()), static_cast<int>(value));
    }

    void ShaderProgram::setInt(const str& name, int value) const {
        glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
    }

    void ShaderProgram::setFloat(const str& name, float value) const {
        glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
    }

    void ShaderProgram::setVec2(const str& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
    }

    void ShaderProgram::setVec2(const str& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(_id, name.c_str()), x, y);
    }

    void ShaderProgram::setVec3(const str& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
    }

    void ShaderProgram::setVec3(const str& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(_id, name.c_str()), x, y, z);
    }

    void ShaderProgram::setVec4(const str& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
    }

    void ShaderProgram::setVec4(const str& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(_id, name.c_str()), x, y, z, w);
    }

    void ShaderProgram::setMat2(const str& name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void ShaderProgram::setMat3(const str& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void ShaderProgram::setMat4(const str& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
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
}  // namespace x::Graphics