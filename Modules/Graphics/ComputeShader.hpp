// Author: Jake Rieger
// Created: 12/20/2024.
//

#pragma once

#include "Shader.hpp"
#include "Types.hpp"
#include <glad.h>

namespace x::Graphics {
    class ComputeShader : public Shader {
    public:
        explicit ComputeShader(const str& source) : Shader(ShaderType::Compute, source) {}
        void dispatch(GLuint x, GLuint y, GLuint z) const;
    };
}  // namespace x::Graphics