set(FILESYSTEM_SRCS
        ${MODULES}/Filesystem/Filesystem.hpp
        ${MODULES}/Filesystem/Filesystem.cpp
)

set(FILESYSTEM_TESTS
        ${MODULES}/Filesystem/Filesystem.Tests.cpp
)

add_executable(Tests.Filesystem ${FILESYSTEM_SRCS} ${FILESYSTEM_TESTS})
find_package(Catch2 3 REQUIRED)
target_link_libraries(Tests.Filesystem PRIVATE
        Catch2::Catch2WithMain
)