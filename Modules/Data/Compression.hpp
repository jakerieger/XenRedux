// Author: Jake Rieger
// Created: 12/13/2024.
//

#pragma once

#include "BinaryData.hpp"
#include "CompressedData.hpp"

namespace x::Compression {
    class LZMA {
    public:
        static CompressedData Compress(const void* data, size_t size);
        static CompressedData Compress(const std::vector<u8>& data);
        static CompressedData Compress(const BinaryData& data);
        static BinaryData Decompress(const CompressedData& data);
    };

    class GZip {
    public:
        static CompressedData Compress(const void* data, size_t size);
        static CompressedData Compress(const std::vector<u8>& data);
        static CompressedData Compress(const BinaryData& data);
        static BinaryData Decompress(const CompressedData& data);
    };
}  // namespace x::Compression
