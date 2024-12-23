// Author: Jake Rieger
// Created: 12/21/2024.
//

#pragma once

#include "Types.hpp"
#include <glad.h>
#include <iostream>

namespace x::Graphics {
    static void APIENTRY glDebugOutput(GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       GLsizei length,
                                       const GLchar* message,
                                       const void* userParam) {
        // Ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        std::cout << "---------------" << std::endl;
        std::cout << "Debug message (" << id << "): " << message << std::endl;

        std::cout << "Source: ";
        switch (source) {
            case GL_DEBUG_SOURCE_API:
                std::cout << "API";
                break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                std::cout << "Window System";
                break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                std::cout << "Shader Compiler";
                break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                std::cout << "Third Party";
                break;
            case GL_DEBUG_SOURCE_APPLICATION:
                std::cout << "Application";
                break;
            case GL_DEBUG_SOURCE_OTHER:
                std::cout << "Other";
                break;
        }
        std::cout << std::endl;

        std::cout << "Type: ";
        switch (type) {
            case GL_DEBUG_TYPE_ERROR:
                std::cout << "Error";
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                std::cout << "Deprecated Behavior";
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                std::cout << "Undefined Behavior";
                break;
            case GL_DEBUG_TYPE_PORTABILITY:
                std::cout << "Portability";
                break;
            case GL_DEBUG_TYPE_PERFORMANCE:
                std::cout << "Performance";
                break;
            case GL_DEBUG_TYPE_MARKER:
                std::cout << "Marker";
                break;
            case GL_DEBUG_TYPE_PUSH_GROUP:
                std::cout << "Push Group";
                break;
            case GL_DEBUG_TYPE_POP_GROUP:
                std::cout << "Pop Group";
                break;
            case GL_DEBUG_TYPE_OTHER:
                std::cout << "Other";
                break;
        }
        std::cout << std::endl;

        std::cout << "Severity: ";
        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                std::cout << "High";
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                std::cout << "Medium";
                break;
            case GL_DEBUG_SEVERITY_LOW:
                std::cout << "Low";
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                std::cout << "Notification";
                break;
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    static void enableDebugOutput() {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);  // Makes callbacks synchronous
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    static void checkError(const char* file, int line) {
        GLenum error;
        while ((error = glGetError()) != GL_NO_ERROR) {
            std::cout << file << ":" << line << '\n';
            std::cout << "OpenGL Error: " << error << std::endl;
        }
    }
}  // namespace x::Graphics

#define CHECK_GL_ERROR() x::Graphics::checkError(__FILE__, __LINE__)