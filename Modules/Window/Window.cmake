set(WINDOW_TESTS
        ${MODULES}/Window/Window.Tests.cpp
)

if (WIN32)
    set(WINDOW_SRCS
            ${MODULES}/Window/Window.hpp
            ${MODULES}/Window/WindowEvents.hpp
            ${MODULES}/Window/Win32/Win32Window.hpp
            ${MODULES}/Window/Win32/Win32Window.cpp
    )

    add_executable(Tests.Window
            ${WINDOW_SRCS}
            ${WINDOW_TESTS}
    )

    find_package(Catch2 3 REQUIRED)
    target_link_libraries(Tests.Window PRIVATE
            Catch2::Catch2
    )
elseif (UNIX)

endif ()