// Author: Jake Rieger
// Created: 12/13/2024.
//

#include <iostream>
#include <catch2/catch_test_macros.hpp>

#include "Filesystem/Filesystem.hpp"
#include "Data/CompressedData.hpp"
#include "Data/Compression.hpp"

using namespace x;

static const str oneMb = "TestData/1MB.bin";
static const str tenMb = "TestData/10MB.bin";
// static const str hundredMb = "TestData/100MB.bin";
// static const str oneGb     = "TestData/1000MB.bin";

TEST_CASE("Compression - GZip", "[Data]") {
    using Compression::GZip;
    using Filesystem::FileReader;

    std::vector<u8> smallData = FileReader::readAllBytes(oneMb);
    std::vector<u8> largeData = FileReader::readAllBytes(tenMb);
    REQUIRE(!smallData.empty());
    REQUIRE(!largeData.empty());

    const CompressedData compressedSmall = GZip::Compress(smallData.data(), smallData.size());
    REQUIRE(compressedSmall.getData() != nullptr);
    REQUIRE(compressedSmall.getOriginalSize() == smallData.size());
    REQUIRE(compressedSmall.getCompressionType() == CompressionType::GZIP);

    const CompressedData compressedLarge = GZip::Compress(largeData.data(), largeData.size());
    REQUIRE(compressedLarge.getData() != nullptr);
    REQUIRE(compressedLarge.getOriginalSize() == largeData.size());
    REQUIRE(compressedLarge.getCompressionType() == CompressionType::GZIP);
}

TEST_CASE("Compression - LZMA", "[Data]") {
    using Compression::LZMA;
    using Filesystem::FileReader;

    std::vector<u8> smallData = FileReader::readAllBytes(oneMb);
    std::vector<u8> largeData = FileReader::readAllBytes(tenMb);
    REQUIRE(!smallData.empty());
    REQUIRE(!largeData.empty());

    const CompressedData compressedSmall = LZMA::Compress(smallData.data(), smallData.size());
    REQUIRE(compressedSmall.getData() != nullptr);
    REQUIRE(compressedSmall.getOriginalSize() == smallData.size());
    REQUIRE(compressedSmall.getCompressionType() == CompressionType::LZMA);

    const CompressedData compressedLarge = LZMA::Compress(largeData.data(), largeData.size());
    REQUIRE(compressedLarge.getData() != nullptr);
    REQUIRE(compressedLarge.getOriginalSize() == largeData.size());
    REQUIRE(compressedLarge.getCompressionType() == CompressionType::LZMA);
}