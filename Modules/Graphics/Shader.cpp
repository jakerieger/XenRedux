// Author: Jake Rieger
// Created: 12/20/2024.
//

#include "Shader.hpp"
#include "Panic.hpp"

namespace x::Graphics {
    Shader::Shader(ShaderType shaderType, const str& shaderSource)
        : _shaderType(shaderType), _shaderSource(shaderSource) {
        compile();
    }

    Shader::~Shader() {
        if (_id > 0) glDeleteShader(_id);
    }

    GLuint Shader::getId() const {
        return _id;
    }

    const str& Shader::getSource() const {
        return _shaderSource;
    }

    ShaderType Shader::getType() const {
        return _shaderType;
    }

    bool Shader::compile() {
        _id                = glCreateShader(shaderTypeToEnum(_shaderType));
        const char* source = _shaderSource.c_str();
        glShaderSource(_id, 1, &source, nullptr);
        glCompileShader(_id);
        checkErrors();
        return true;
    }

    void Shader::checkErrors() const {
        int success;
        char infoLog[1024];
        glGetShaderiv(_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(_id, 1024, nullptr, infoLog);
            const char* type = _shaderType == ShaderType::Vertex     ? "Vertex"
                               : _shaderType == ShaderType::Fragment ? "Fragment"
                                                                     : "Compute";
            Panic("Shader compilation error (%s):\n%s", type, infoLog);
        }
    }

    GLenum Shader::shaderTypeToEnum(const ShaderType shaderType) {
        switch (shaderType) {
            case ShaderType::Vertex: {
                return GL_VERTEX_SHADER;
            }
            case ShaderType::Fragment: {
                return GL_FRAGMENT_SHADER;
            }
            case ShaderType::Compute: {
                return GL_COMPUTE_SHADER;
            }
            default:
                return false;
        }
    }
}  // namespace x::Graphics