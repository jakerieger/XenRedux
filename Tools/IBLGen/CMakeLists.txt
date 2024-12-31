project(XEN)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tools/IBLGen)

add_executable(IBLGen
        ${GLAD_SRCS}
        ${STB_SRCS}
        ${IMGUI_SRCS}
        main.cpp
)

target_link_libraries(IBLGen PRIVATE
        Xen
        glfw
        glm::glm-header-only
)