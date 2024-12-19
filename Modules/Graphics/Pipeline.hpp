// Author: Jake Rieger
// Created: 12/19/2024.
//

#pragma once

namespace x::Graphics {
    class Pipeline {
    public:
        static void setBlendMode(bool enable);
        static void setDepthTest(bool enable);
        static void setCullMode(bool enable);
        static void setPolygonMode(bool wireframe);
    };
}  // namespace x::Graphics