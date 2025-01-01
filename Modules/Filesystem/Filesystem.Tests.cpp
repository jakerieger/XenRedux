// Author: Jake Rieger
// Created: 12/12/2024.
//

#include <catch2/catch_test_macros.hpp>

#include "Filesystem.hpp"

using namespace x;

static const str oneMb     = "TestData/1MB.bin";
static const str tenMb     = "TestData/10MB.bin";
static const str hundredMb = "TestData/100MB.bin";
static const str oneGb     = "TestData/1000MB.bin";

TEST_CASE("Filesystem I/O (Reading)", "[Filesystem]") {
    using Filesystem::FileReader;

    const auto bytes1mb = FileReader::readAllBytes(oneMb);
    REQUIRE(!bytes1mb.empty());
    REQUIRE(FileReader::queryFileSize(oneMb) == bytes1mb.size());

    const auto bytes10mb = FileReader::readAllBytes(tenMb);
    REQUIRE(!bytes10mb.empty());
    REQUIRE(FileReader::queryFileSize(tenMb) == bytes10mb.size());

    const auto bytes100mb = FileReader::readAllBytes(hundredMb);
    REQUIRE(!bytes100mb.empty());
    REQUIRE(FileReader::queryFileSize(hundredMb) == bytes100mb.size());

    const auto bytes1gb = FileReader::readAllBytes(oneGb);
    REQUIRE(!bytes1gb.empty());
    REQUIRE(FileReader::queryFileSize(oneGb) == bytes1gb.size());
}

TEST_CASE("Filesystem I/O (Writing)", "[Filesystem]") {
    using Filesystem::FileWriter;
}

TEST_CASE("Filesystem::Path", "[Filesystem]") {
    using Filesystem::Path;
    const Path cwd = Path::currentPath();
    str pathStr    = cwd.str();
    REQUIRE(!pathStr.empty());
    REQUIRE(cwd.isDirectory());
    auto testData1Mb = cwd / oneMb;
    REQUIRE(testData1Mb.exists());
    REQUIRE(testData1Mb.isFile());
    REQUIRE(!testData1Mb.isDirectory());
    REQUIRE(testData1Mb.hasExtension());
    // TODO: Works on linux, not Windows
    // REQUIRE(testData1Mb.parent() == cwd.join("TestData"));
    auto newTestData = testData1Mb.replaceExtension("bin1");
    REQUIRE(newTestData.hasExtension());
    REQUIRE(newTestData.extension() == "bin1");
}