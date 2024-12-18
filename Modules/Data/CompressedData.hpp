// Author: Jake Rieger
// Created: 12/13/2024.
//

#pragma once

#include "Data.hpp"
#include <vector>

namespace x {
    enum class CompressionType {
        LZMA,
        GZIP,
    };

    class CompressedData final : public IData {
    public:
        CompressedData(const void* data,
                       size_t size,
                       size_t originalSize,
                       CompressionType compressionType = CompressionType::LZMA);
        explicit CompressedData(const std::vector<u8>& data,
                                size_t originalSize,
                                CompressionType compressionType = CompressionType::LZMA);
        CompressedData(const CompressedData& data);
        ~CompressedData() override;

        CompressedData* clone() const override;
        void* getData() const override;
        size_t getSize() const override;
        size_t getOriginalSize() const;
        CompressionType getCompressionType() const;

    private:
        void _init();
        char* _data;
        size_t _size;
        size_t _originalSize;
        CompressionType _type;
    };
}  // namespace x
