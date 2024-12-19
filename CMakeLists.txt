cmake_minimum_required(VERSION 3.14)

# These need to be set before "project" is called
set(CMAKE_TOOLCHAIN_FILE vcpkg/scripts/buildsystems/vcpkg.cmake)

project(XEN)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(COMMON ${CMAKE_CURRENT_SOURCE_DIR}/Common)
set(MODULES ${CMAKE_CURRENT_SOURCE_DIR}/Modules)
set(VENDOR ${CMAKE_CURRENT_SOURCE_DIR}/Vendor)

include_directories(
        ${COMMON}
        ${MODULES}
        ${VENDOR}
)

# Vendor
include(Vendor/Vendor.cmake)

# Modules
include(Modules/Audio/Audio.cmake)
include(Modules/Data/Data.cmake)
include(Modules/Event/Event.cmake)
include(Modules/Filesystem/Filesystem.cmake)
include(Modules/Font/Font.cmake)
include(Modules/Graphics/Graphics.cmake)
include(Modules/Image/Image.cmake)
include(Modules/Keyboard/Keyboard.cmake)
include(Modules/Math/Math.cmake)
include(Modules/Memory/Memory.cmake)
include(Modules/Mouse/Mouse.cmake)
include(Modules/Physics/Physics.cmake)
include(Modules/System/System.cmake)
include(Modules/Thread/Thread.cmake)
include(Modules/Window/Window.cmake)

find_package(liblzma CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

add_library(Xen STATIC
        ${GLAD_SRCS}
        ${COMMON}/Types.hpp
        ${COMMON}/Panic.hpp
        ${FILESYSTEM_SRCS}
        ${DATA_SRCS}
        ${MEMORY_SRCS}
        ${GRAPHICS_SRCS}
)

target_link_libraries(Xen PRIVATE
        liblzma::liblzma
        ZLIB::ZLIB
)

# Copy test data to bin directory for unit tests
add_custom_command(TARGET Xen PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources/TestData ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TestData)