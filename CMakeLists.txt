cmake_minimum_required(VERSION 3.14)

# These need to be set before "project" is called
set(CMAKE_TOOLCHAIN_FILE vcpkg/scripts/buildsystems/vcpkg.cmake)

project(XEN)

set(CMAKE_CXX_STANDARD 20)
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
include(Modules/Input/Input.cmake)
include(Modules/Math/Math.cmake)
include(Modules/Memory/Memory.cmake)
include(Modules/Physics/Physics.cmake)
include(Modules/System/System.cmake)
include(Modules/Thread/Thread.cmake)

find_package(liblzma CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(xxHash CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

include_directories(
        ${VENDOR}/imgui
        ${VENDOR}/imgui/backends
)

add_library(Xen STATIC
        # Vendor sources
        ${GLAD_SRCS}
        ${STB_SRCS}
        ${IMGUI_SRCS}
        # Modules
        ${DATA_SRCS}
        ${FILESYSTEM_SRCS}
        ${GRAPHICS_SRCS}
        ${MEMORY_SRCS}
        ${MATH_SRCS}
        # Engine core
        ${COMMON}/BlinnPhongMaterial.cpp
        ${COMMON}/BlinnPhongMaterial.hpp
        ${COMMON}/Camera.hpp
        ${COMMON}/Clock.cpp
        ${COMMON}/Clock.hpp
        ${COMMON}/Context.cpp
        ${COMMON}/Context.hpp
        ${COMMON}/DirectionalLight.cpp
        ${COMMON}/DirectionalLight.hpp
        ${COMMON}/EventSystem.hpp
        ${COMMON}/Game.cpp
        ${COMMON}/Game.hpp
        ${COMMON}/Light.cpp
        ${COMMON}/Light.hpp
        ${COMMON}/Material.cpp
        ${COMMON}/Material.hpp
        ${COMMON}/Mesh.cpp
        ${COMMON}/Mesh.hpp
        ${COMMON}/Model.cpp
        ${COMMON}/Model.hpp
        ${COMMON}/PBRMaterial.cpp
        ${COMMON}/PBRMaterial.hpp
        ${COMMON}/Panic.hpp
        ${COMMON}/PerspectiveCamera.cpp
        ${COMMON}/PerspectiveCamera.hpp
        ${COMMON}/PointLight.cpp
        ${COMMON}/PointLight.hpp
        ${COMMON}/ShaderManager.cpp
        ${COMMON}/ShaderManager.hpp
        ${COMMON}/Transform.cpp
        ${COMMON}/Transform.hpp
        ${COMMON}/Types.hpp
        ${COMMON}/Volatile.hpp
)

if (WIN32)
    target_link_libraries(Xen PRIVATE
            liblzma::liblzma
            ZLIB::ZLIB
            glm::glm-header-only
            assimp::assimp
            xxHash::xxhash
            glfw
    )
elseif (UNIX)
    target_link_libraries(Xen PRIVATE
            liblzma::liblzma
            ZLIB::ZLIB
            glm::glm-header-only
            assimp::assimp
            xxHash::xxhash
            glfw
    )
endif ()

# Game project
add_subdirectory(SpaceGame)