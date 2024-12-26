set(GLAD_SRCS
        ${VENDOR}/glad.h
        ${VENDOR}/glad.c
)

set(STB_SRCS
        ${VENDOR}/stb_image.h
        ${VENDOR}/stb_image.c
)

file(GLOB IMGUI_SRCS
        ${VENDOR}/imgui/*.h
        ${VENDOR}/imgui/*.cpp
        ${VENDOR}/imgui/backends/imgui_impl_glfw*
        ${VENDOR}/imgui/backends/imgui_impl_opengl3*
)