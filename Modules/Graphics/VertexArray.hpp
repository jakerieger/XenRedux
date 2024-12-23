// Author: Jake Rieger
// Created: 12/22/2024.
//

#pragma once

#include <glad.h>
#include "Types.hpp"
#include "Panic.hpp"
#include "Memory/GpuBuffer.hpp"

#include <vector>

namespace x::Graphics {
    struct VertexAttribute {
        GLuint index;
        GLsizei size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        const void* offset;
    };

    class VertexArray {
    public:
        VertexArray(const std::vector<VertexAttribute>& attributes,
                    const std::vector<f32>& vertices,
                    const std::vector<u32>& indices);
        ~VertexArray();

        void bind() const;
        void enableAttribute(const VertexAttribute& attr) const;
        void disableAttribute(const VertexAttribute& attr) const;
        void cleanup();

        void bindVertex() const;
        void bindIndex() const;

        static void unbind();
        static void unbindBuffers();

    private:
        GLuint _vao;
        std::unique_ptr<Memory::VertexBuffer> _vertexBuffer;
        std::unique_ptr<Memory::IndexBuffer> _indexBuffer;
        std::vector<VertexAttribute> _attributes;
        bool _cleanedUp = false;
    };
}  // namespace x::Graphics