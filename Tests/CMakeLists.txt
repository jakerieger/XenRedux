project(XEN)

add_executable(Tests.Rendering
        Tests.Rendering.cpp
)

target_link_libraries(Tests.Rendering PRIVATE Xen)