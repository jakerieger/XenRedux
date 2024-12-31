project(XEN)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tools/IBLGen)

add_executable(IBLGen
        ${GLAD_SRCS}
        ${STB_SRCS}
        ${IMGUI_SRCS}
        main.cpp
)

find_package(tinyfiledialogs CONFIG REQUIRED)

target_link_libraries(IBLGen PRIVATE
        Xen
        glfw
        glm::glm-header-only
        tinyfiledialogs::tinyfiledialogs
        OpenEXR::OpenEXR
)