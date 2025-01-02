// Author: Jake Rieger
// Created: 12/20/2024.
//

#pragma once

#include "Graphics/ShaderProgram.hpp"

#include <unordered_map>
#include <memory>

namespace x {
    class ShaderManager {
    public:
        ShaderManager(const ShaderManager&)            = delete;
        ShaderManager& operator=(const ShaderManager&) = delete;
        ShaderManager(ShaderManager&&)                 = delete;
        ShaderManager& operator=(ShaderManager&&)      = delete;

        static ShaderManager& instance();

        std::shared_ptr<Graphics::ShaderProgram> getShaderProgram(const str& vertexSource,
                                                                  const str& fragmentSource);
        std::shared_ptr<Graphics::ShaderProgram> getShaderProgram(const str& computeSource);

    private:
        ShaderManager() = default;

        std::shared_ptr<Graphics::ShaderProgram> createProgram(const str& vertexSource,
                                                               const str& fragmentSource) const;
        std::shared_ptr<Graphics::ShaderProgram> createProgram(const str& computeSource) const;

        u64 getHash(const str& source) const;

        std::unordered_map<u64, std::shared_ptr<Graphics::ShaderProgram>> _cache;
    };
}  // namespace x
