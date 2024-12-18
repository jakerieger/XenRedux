// Author: Jake Rieger
// Created: 12/13/2024.
//

#pragma once

#include "Data.hpp"
#include <vector>

namespace x {
    class BinaryData final : public IData {
    public:
        explicit BinaryData(size_t size, bool clear = true);
        BinaryData(const void* data, size_t size);
        explicit BinaryData(const std::vector<u8>& data);
        BinaryData(const BinaryData& data);
        ~BinaryData() override;

        BinaryData* clone() const override;
        void* getData() const override;
        size_t getSize() const override;

    private:
        void _init();
        char* _data;
        size_t _size;
    };
}  // namespace x
