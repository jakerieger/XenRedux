// Author: Jake Rieger
// Created: 12/22/2024.
//

#pragma once

#include <glad.h>
#include "Types.hpp"
#include "Panic.hpp"
#include "Vertex.hpp"
#include "Memory/GpuBuffer.hpp"
#include "VertexAttribute.hpp"
#include "DebugOpenGL.hpp"

#include <vector>

namespace x::Graphics {
    template<typename V, typename I = uint32_t>
    class VertexArray {
    public:
        VertexArray(const std::vector<VertexAttribute>& attributes,
                    const std::vector<V>& vertices,
                    const std::vector<I>& indices);
        ~VertexArray();

        void bind() const;
        void enableAttribute(const VertexAttribute& attribute) const;
        void disableAttribute(const VertexAttribute& attribute) const;
        void cleanup();

        void bindVertexBuffer() const;
        void bindIndexBuffer() const;

        static void unbind();
        static void unbindVertexBuffer();
        static void unbindIndexBuffer();
        static void unbindAll();

    private:
        GLuint _vao;
        std::unique_ptr<Memory::GpuBuffer<V, Memory::Vertex>> _vertexBuffer;
        std::unique_ptr<Memory::GpuBuffer<I, Memory::Index>> _indexBuffer;
        std::vector<VertexAttribute> _attributes;
        bool _cleanedUp = false;
    };

    template<typename V, typename I>
    VertexArray<V, I>::VertexArray(const std::vector<VertexAttribute>& attributes,
                                   const std::vector<V>& vertices,
                                   const std::vector<I>& indices) {
        _attributes   = attributes;
        _vertexBuffer = std::make_unique<Memory::GpuBuffer<V, Memory::Vertex>>(vertices);
        _indexBuffer  = std::make_unique<Memory::GpuBuffer<I, Memory::Index>>(indices);
        glGenVertexArrays(1, &_vao);
        CHECK_GL_ERROR();
        for (const auto& attribute : _attributes) {
            enableAttribute(attribute);
        }
    }

    template<typename V, typename I>
    VertexArray<V, I>::~VertexArray() {
        if (!_cleanedUp) cleanup();
    }

    template<typename V, typename I>
    void VertexArray<V, I>::bind() const {
        glBindVertexArray(_vao);
        CHECK_GL_ERROR();
    }

    template<typename V, typename I>
    void VertexArray<V, I>::enableAttribute(const VertexAttribute& attribute) const {
        bindVertexBuffer();
        bind();
        glEnableVertexAttribArray(attribute.index);
        glVertexAttribPointer(attribute.index,
                              attribute.size,
                              attribute.type,
                              attribute.normalized,
                              attribute.stride,
                              attribute.offset);
        CHECK_GL_ERROR();
    }

    template<typename V, typename I>
    void VertexArray<V, I>::disableAttribute(const VertexAttribute& attribute) const {
        bindVertexBuffer();
        bind();
        glDisableVertexAttribArray(attribute.index);
        CHECK_GL_ERROR();
    }

    template<typename V, typename I>
    void VertexArray<V, I>::cleanup() {
        glDeleteVertexArrays(1, &_vao);
        CHECK_GL_ERROR();
        _cleanedUp = true;
    }

    template<typename V, typename I>
    void VertexArray<V, I>::bindVertexBuffer() const {
        _vertexBuffer->bind();
    }

    template<typename V, typename I>
    void VertexArray<V, I>::bindIndexBuffer() const {
        _indexBuffer->bind();
    }

    template<typename V, typename I>
    void VertexArray<V, I>::unbind() {
        glBindVertexArray(0);
    }

    template<typename V, typename I>
    void VertexArray<V, I>::unbindVertexBuffer() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR();
    }

    template<typename V, typename I>
    void VertexArray<V, I>::unbindIndexBuffer() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR();
    }

    template<typename V, typename I>
    void VertexArray<V, I>::unbindAll() {
        unbind();
        unbindVertexBuffer();
        unbindIndexBuffer();
    }
}  // namespace x::Graphics