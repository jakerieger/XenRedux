set(DATA_SRCS
        ${MODULES}/Data/Data.hpp
        ${MODULES}/Data/Data.cpp
        ${MODULES}/Data/BinaryData.hpp
        ${MODULES}/Data/BinaryData.cpp
        ${MODULES}/Data/CompressedData.hpp
        ${MODULES}/Data/CompressedData.cpp
        ${MODULES}/Data/Compression.hpp
        ${MODULES}/Data/Compression.cpp
)

set(DATA_TESTS
        ${MODULES}/Data/Data.Tests.cpp
)

add_executable(Tests.Data
        ${DATA_SRCS}
        ${DATA_TESTS}
        ${MODULES}/Filesystem/Filesystem.hpp
        ${MODULES}/Filesystem/Filesystem.cpp
)

find_package(Catch2 3 REQUIRED)
target_link_libraries(Tests.Data PRIVATE
        ZLIB::ZLIB
        liblzma::liblzma
        Catch2::Catch2WithMain
)