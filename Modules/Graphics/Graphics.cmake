set(GRAPHICS_SRCS
        ${MODULES}/Graphics/Renderer.hpp
        ${MODULES}/Graphics/Buffer.hpp
        ${MODULES}/Graphics/OpenGL/OpenGLBuffer.cpp
        ${MODULES}/Graphics/OpenGL/OpenGLBuffer.hpp
        ${MODULES}/Graphics/OpenGL/OpenGLRenderer.cpp
        ${MODULES}/Graphics/OpenGL/OpenGLRenderer.hpp
        ${MODULES}/Graphics/OpenGL/OpenGLPipelineState.cpp
        ${MODULES}/Graphics/OpenGL/OpenGLPipelineState.hpp
        ${MODULES}/Graphics/OpenGL/OpenGLMaterial.cpp
        ${MODULES}/Graphics/OpenGL/OpenGLMaterial.hpp
)

set(GRAPHICS_TESTS
        ${MODULES}/Graphics/Graphics.Tests.cpp
)

add_executable(Tests.Graphics
        ${GLAD_SRCS}
        ${GRAPHICS_SRCS}
        ${GRAPHICS_TESTS}
)

find_package(Catch2 3 REQUIRED)
target_link_libraries(Tests.Graphics PRIVATE
        Catch2::Catch2WithMain
)