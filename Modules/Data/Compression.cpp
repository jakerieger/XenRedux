// Author: Jake Rieger
// Created: 12/13/2024.
//

#pragma warning(disable : 4267)

#include "Compression.hpp"

#include <lzma.h>
#include <zlib.h>

namespace x::Compression {
#pragma region LZMA
    CompressedData LZMA::compress(const void* data, size_t size) {
        if (!data || !size) { throw std::invalid_argument("data is null"); }
        lzma_stream strm = LZMA_STREAM_INIT;
        lzma_ret ret     = lzma_easy_encoder(&strm, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC32);
        if (ret != LZMA_OK) { throw std::runtime_error("lzma_easy_encoder failed"); }

        size_t compressedSize = size + size / 3 + 128;
        const auto compressed = new u8[compressedSize];

        strm.next_in   = (u8*)data;
        strm.avail_in  = size;
        strm.next_out  = (u8*)compressed;
        strm.avail_out = compressedSize;

        ret = lzma_code(&strm, LZMA_FINISH);
        if (ret != LZMA_STREAM_END) {
            lzma_end(&strm);
            throw std::runtime_error("lzma_code failed");
        }

        compressedSize = strm.total_out;
        lzma_end(&strm);

        const auto compressedData =
          CompressedData(compressed, compressedSize, size, CompressionType::LZMA);

        return compressedData;
    }

    CompressedData LZMA::compress(const std::vector<u8>& data) {
        return compress(data.data(), data.size());
    }

    CompressedData LZMA::compress(const BinaryData& data) {
        return compress(data.getData(), data.getSize());
    }

    BinaryData LZMA::decompress(const CompressedData& data) {
        if (data.getSize() == 0 || data.getData() == nullptr) {
            throw std::invalid_argument("data is null");
        }

        lzma_stream strm = LZMA_STREAM_INIT;
        lzma_ret ret     = lzma_auto_decoder(&strm, UINT64_MAX, 0);
        if (ret != LZMA_OK) { throw std::runtime_error("lzma_auto_decoder failed"); }

        const auto originalSize = data.getOriginalSize();
        const auto decompressed = new u8[originalSize];

        strm.next_in   = (u8*)data.getData();
        strm.avail_in  = data.getSize();
        strm.next_out  = decompressed;
        strm.avail_out = originalSize;

        ret = lzma_code(&strm, LZMA_FINISH);
        if (ret != LZMA_STREAM_END) {
            lzma_end(&strm);
            throw std::runtime_error("lzma_code failed");
        }
        lzma_end(&strm);

        auto decompressedData = BinaryData(decompressed, originalSize);
        return decompressedData;
    }
#pragma endregion

#pragma region GZip
    CompressedData GZip::compress(const void* data, size_t size) {
        if (!data || !size) { throw std::invalid_argument("data is null"); }
        auto compressedSize   = compressBound(size);
        const auto compressed = new u8[compressedSize];
        const auto result = ::compress(compressed, &compressedSize, CAST<const u8*>(data), size);
        if (result != Z_OK) { throw std::runtime_error("compress failed"); }

        const auto compressedData =
          CompressedData(compressed, compressedSize, size, CompressionType::GZIP);
        return compressedData;
    }

    CompressedData GZip::compress(const std::vector<u8>& data) {
        return compress(data.data(), data.size());
    }

    CompressedData GZip::compress(const BinaryData& data) {
        return compress(data.getData(), data.getSize());
    }

    BinaryData GZip::decompress(const CompressedData& data) {
        if (data.getSize() == 0 || data.getData() == nullptr) {
            throw std::invalid_argument("data is null");
        }

        uLongf decompressedSize = data.getOriginalSize();
        const auto decompressed = new u8[decompressedSize];
        const auto result       = uncompress(decompressed,
                                       &decompressedSize,
                                       CAST<const u8*>(data.getData()),
                                       data.getSize());

        if (result != Z_OK) { throw std::runtime_error("uncompress failed"); }
        const auto decompressedData = BinaryData(decompressed, decompressedSize);
        return decompressedData;
    }
#pragma endregion
}  // namespace x::Compression