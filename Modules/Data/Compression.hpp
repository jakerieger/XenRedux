// Author: Jake Rieger
// Created: 12/13/2024.
//

#pragma once

#include "BinaryData.hpp"
#include "CompressedData.hpp"

namespace x::Compression {
    class LZMA {
    public:
        static CompressedData compress(const void* data, size_t size);
        static CompressedData compress(const std::vector<u8>& data);
        static CompressedData compress(const BinaryData& data);
        static BinaryData decompress(const CompressedData& data);
    };

    class GZip {
    public:
        static CompressedData compress(const void* data, size_t size);
        static CompressedData compress(const std::vector<u8>& data);
        static CompressedData compress(const BinaryData& data);
        static BinaryData decompress(const CompressedData& data);
    };
}  // namespace x::Compression
