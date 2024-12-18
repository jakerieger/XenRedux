// Author: Jake Rieger
// Created: 12/12/24.
//

#pragma once

#include "Types.hpp"
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#ifdef _WIN32
    #include <direct.h>
    #define getcwd _getcwd
    #define PATH_SEPARATOR '\\'
#else
    #include <unistd.h>
    #define PATH_SEPARATOR '/'
#endif

namespace x {
    namespace Filesystem {
        class FileReader {
        public:
            static std::vector<u8> readAllBytes(const str& path);
            static str readAllText(const str& path);
            static std::vector<str> readAllLines(const str& path);
            static std::vector<u8> readBlock(const str& path, size_t size, u64 offset = 0);
            static size_t queryFileSize(const str& path);
        };

        class FileWriter {
        public:
            static bool writeAllBytes(const str& path, const std::vector<u8>& data);
            static bool writeAllText(const str& path, const str& text);
            static bool writeAllLines(const str& path, const std::vector<str>& lines);
            static bool writeBlock(const str& path, const std::vector<u8>& data, u64 offset = 0);
        };

        class StreamReader {};

        class StreamWriter {};

        class Path {
        public:
            explicit Path(const str& path) : path(normalize(path)) {}
            static Path currentPath();

            [[nodiscard]] Path parent() const;
            [[nodiscard]] bool exists() const;
            [[nodiscard]] bool isFile() const;
            [[nodiscard]] bool isDirectory() const;
            [[nodiscard]] bool hasExtension() const;
            [[nodiscard]] str extension() const;
            [[nodiscard]] Path replaceExtension(const str& ext) const;
            [[nodiscard]] Path join(const str& subPath) const;
            [[nodiscard]] Path operator/(const str& subPath) const;
            [[nodiscard]] str toString() const;
            [[nodiscard]] bool operator==(const Path& other) const;

        private:
            str path;
            static str join(const str& lhs, const str& rhs);
            static str normalize(const str& rawPath);
        };
    };  // namespace Filesystem
}  // namespace x
