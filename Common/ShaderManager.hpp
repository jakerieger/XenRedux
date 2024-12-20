// Author: Jake Rieger
// Created: 12/20/2024.
//

#pragma once

#include "Graphics/ShaderProgram.hpp"
#include "Graphics/ComputeShader.hpp"

namespace x {
    class ShaderManager {
    public:
        static std::shared_ptr<Graphics::ShaderProgram> createProgram(const str& vertexSource,
                                                                      const str& fragmentSource);
        static std::shared_ptr<Graphics::ComputeShader>
        createComputeShader(const str& computeSource);

    private:
        ShaderManager() = default;
    };
}  // namespace x
