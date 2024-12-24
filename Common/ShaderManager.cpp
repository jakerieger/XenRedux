// Author: Jake Rieger
// Created: 12/20/2024.
//

#include "ShaderManager.hpp"

namespace x {
    std::shared_ptr<Graphics::ShaderProgram>
    ShaderManager::createProgram(const str& vertexSource, const str& fragmentSource) {
        const Graphics::Shader vertexShader(Graphics::ShaderType::Vertex, vertexSource);
        const Graphics::Shader fragmentShader(Graphics::ShaderType::Fragment, fragmentSource);
        auto program = std::make_shared<Graphics::ShaderProgram>();
        program->attachShader(vertexShader);
        program->attachShader(fragmentShader);
        program->link();
        return program;
    }

    std::shared_ptr<Graphics::ShaderProgram>
    ShaderManager::createProgram(const str& computeSource) {
        return nullptr;
    }
}  // namespace x