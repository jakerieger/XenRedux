// Author: Jake Rieger
// Created: 12/20/2024.
//

#include "Shader.hpp"
#include "Panic.hpp"

namespace x::Graphics {
    Shader::Shader(ShaderType shaderType, const str& shaderSource)
        : _shaderType(shaderType == ShaderType::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER),
          _shaderSource(shaderSource) {
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

    bool Shader::compile() {
        switch (_shaderType) {
            case GL_VERTEX_SHADER: {
                _id                = glCreateShader(GL_VERTEX_SHADER);
                const char* source = _shaderSource.c_str();
                glShaderSource(_id, 1, &source, nullptr);
                glCompileShader(_id);
                checkErrors();
                return true;
            }
            case GL_FRAGMENT_SHADER: {
                _id                = glCreateShader(GL_FRAGMENT_SHADER);
                const char* source = _shaderSource.c_str();
                glShaderSource(_id, 1, &source, nullptr);
                glCompileShader(_id);
                checkErrors();
                return true;
            }
            default:
                return false;
        }
    }

    void Shader::checkErrors() const {
        int success;
        char infoLog[1024];
        glGetShaderiv(_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(_id, 1024, nullptr, infoLog);
            Panic(infoLog);
        }
    }
}  // namespace x::Graphics