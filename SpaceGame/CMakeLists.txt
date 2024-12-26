project(XEN)

add_executable(SpaceGame
        main.cpp
)

target_link_libraries(SpaceGame PRIVATE Xen glfw)