// Author: Jake Rieger
// Created: 12/20/2024.
//

#pragma once

#include "Types.hpp"

#include <glad.h>

namespace x::Graphics {
    enum class ShaderType {
        Vertex,
        Fragment,
        Compute,
    };

    class Shader {
        friend class ComputeShader;

    public:
        Shader(ShaderType shaderType, const str& shaderSource);
        ~Shader();

        GLuint getId() const;
        const str& getSource() const;
        ShaderType getType() const;

    private:
        GLuint _id;
        ShaderType _shaderType;
        const str _shaderSource;
        bool compile();
        void checkErrors() const;
        static GLenum shaderTypeToEnum(const ShaderType shaderType);
    };
}  // namespace x::Graphics