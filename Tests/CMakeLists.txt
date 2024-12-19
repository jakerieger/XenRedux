project(XEN)

add_executable(Tests.Rendering
        Tests.Rendering.cpp
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(Tests.Rendering PRIVATE Xen glfw)