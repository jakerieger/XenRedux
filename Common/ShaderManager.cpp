// Author: Jake Rieger
// Created: 12/20/2024.
//

#include "ShaderManager.hpp"

#include <xxhash.h>

namespace x {
    ShaderManager& ShaderManager::instance() {
        static ShaderManager instance;  // Guaranteed to be lazy-initialized and thread-safe
        return instance;
    }

    std::shared_ptr<Graphics::ShaderProgram>
    ShaderManager::getShaderProgram(const str& vertexSource, const str& fragmentSource) {
        const auto hash = getHash(vertexSource + fragmentSource);
        if (_cache.find(hash) == _cache.end()) {
            // Create program
            auto program = createProgram(vertexSource, fragmentSource);
            _cache.insert({hash, program});
        }
        return _cache.at(hash);
    }

    std::shared_ptr<Graphics::ShaderProgram>
    ShaderManager::getShaderProgram(const str& computeSource) {
        const auto hash = getHash(computeSource);
        if (_cache.find(hash) == _cache.end()) {
            // Create program
            auto program = createProgram(computeSource);
            _cache.insert({hash, program});
        }
        return _cache.at(hash);
    }

    std::shared_ptr<Graphics::ShaderProgram>
    ShaderManager::createProgram(const str& vertexSource, const str& fragmentSource) const {
        const Graphics::Shader vertexShader(Graphics::ShaderType::Vertex, vertexSource);
        const Graphics::Shader fragmentShader(Graphics::ShaderType::Fragment, fragmentSource);
        auto program = std::make_shared<Graphics::ShaderProgram>();
        program->attachShader(vertexShader);
        program->attachShader(fragmentShader);
        program->link();
        return program;
    }

    std::shared_ptr<Graphics::ShaderProgram>
    ShaderManager::createProgram(const str& computeSource) const {
        const Graphics::Shader shader(Graphics::ShaderType::Compute, computeSource);
        auto program = std::make_shared<Graphics::ShaderProgram>();
        program->attachShader(shader);
        program->link();
        return program;
    }

    u64 ShaderManager::getHash(const str& source) const {
        const XXH64_hash_t hash = XXH64(source.c_str(), source.size(), 0);
        return hash;
    }

}  // namespace x