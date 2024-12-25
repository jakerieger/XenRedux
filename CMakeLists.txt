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
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(xxHash CONFIG REQUIRED)


add_library(Xen STATIC
        # Vendor sources
        ${GLAD_SRCS}
        ${STB_SRCS}
        # Modules
        ${DATA_SRCS}
        ${FILESYSTEM_SRCS}
        ${GRAPHICS_SRCS}
        ${MEMORY_SRCS}
        ${WINDOW_SRCS}
        # Engine core
        ${COMMON}/Types.hpp
        ${COMMON}/Panic.hpp
        ${COMMON}/Volatile.hpp
        ${COMMON}/Camera.hpp
        ${COMMON}/EventSystem.hpp
        ${COMMON}/RenderSystem.hpp
        ${COMMON}/RenderSystem.cpp
        ${COMMON}/ShaderManager.hpp
        ${COMMON}/ShaderManager.cpp
        ${COMMON}/PerspectiveCamera.hpp
        ${COMMON}/PerspectiveCamera.cpp
        ${COMMON}/Material.hpp
        ${COMMON}/Material.cpp
        ${COMMON}/Mesh.hpp
        ${COMMON}/Mesh.cpp
        ${COMMON}/Model.hpp
        ${COMMON}/Model.cpp
        ${COMMON}/Transform.hpp
        ${COMMON}/Transform.cpp
        ${COMMON}/Clock.hpp
        ${COMMON}/Clock.cpp
        ${COMMON}/Light.hpp
        ${COMMON}/Light.cpp
        ${COMMON}/DirectionalLight.hpp
        ${COMMON}/DirectionalLight.cpp
        ${COMMON}/BlinnPhongMaterial.hpp
        ${COMMON}/BlinnPhongMaterial.cpp
        ${COMMON}/PBRMaterial.hpp
        ${COMMON}/PBRMaterial.cpp
)

if (WIN32)
    target_link_libraries(Xen PRIVATE
            liblzma::liblzma
            ZLIB::ZLIB
            glm::glm-header-only
            assimp::assimp
            xxHash::xxhash
    )
elseif (UNIX)
    target_link_libraries(Xen PRIVATE
            liblzma::liblzma
            ZLIB::ZLIB
            glm::glm-header-only
            assimp::assimp
            xxHash::xxhash
    )
endif ()

# Tests
add_subdirectory(Tests)

# Copy test data to bin directory for unit tests
add_custom_command(TARGET Xen PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources/TestData ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TestData)