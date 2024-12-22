set(MEMORY_SRCS
        ${MODULES}/Memory/PoolAllocator.hpp
        ${MODULES}/Memory/PoolAllocator.cpp
        ${MODULES}/Memory/ArenaAllocator.hpp
        ${MODULES}/Memory/ArenaAllocator.cpp
        ${MODULES}/Memory/GpuBuffer.hpp
)

set(MEMORY_TESTS
        ${MODULES}/Memory/Memory.Tests.cpp
)

add_executable(Tests.Memory
        ${MEMORY_SRCS}
        ${MEMORY_TESTS}
)

find_package(Catch2 3 REQUIRED)
target_link_libraries(Tests.Memory PRIVATE
        Catch2::Catch2WithMain
)