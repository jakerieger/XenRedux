// Author: Jake Rieger
// Created: 12/18/2024.
//

#pragma once

#include "Types.hpp"
#include "Buffer.hpp"

namespace x::Graphics {
    class IPipelineState {
    public:
        virtual ~IPipelineState() = default;
        virtual void apply()      = 0;
    };

    class IMaterial {
    public:
        virtual ~IMaterial()                       = default;
        virtual IPipelineState* getPipelineState() = 0;
        virtual void bind() const                  = 0;
    };

    class IRenderer {
    public:
        virtual ~IRenderer()                                                     = default;
        virtual void initialize()                                                = 0;
        virtual void setViewport(int x, int y, int width, int height)            = 0;
        virtual void setScissor(int x, int y, int width, int height)             = 0;
        virtual void setPipeline(IPipelineState* state)                          = 0;
        virtual void submitDrawCall(IBuffer* vertexBuffer, IBuffer* indexBuffer) = 0;
        virtual void clear(f32 r, f32 g, f32 b, f32 a)                           = 0;
        virtual void present()                                                   = 0;
    };
}  // namespace x::Graphics