// Author: Jake Rieger
// Created: 12/13/2024.
//

#include "BinaryData.hpp"

namespace x {
    BinaryData::BinaryData(size_t size, bool clear) : _data(nullptr), _size(size) {
        _init();
        if (clear) memset(_data, 0, _size);
    }

    BinaryData::BinaryData(const void* data, size_t size) : _data(nullptr), _size(size) {
        _init();
        if (data != nullptr) memcpy(_data, data, _size);
    }

    BinaryData::BinaryData(const std::vector<u8>& data) : _data(nullptr), _size(data.size()) {
        _init();
        memcpy(_data, data.data(), data.size());
    }

    BinaryData::BinaryData(const BinaryData& data) : _data(nullptr), _size(data._size) {
        _init();
        memcpy(_data, data._data, _size);
    }

    BinaryData::~BinaryData() {
        delete[] _data;
    }

    BinaryData* BinaryData::clone() const {
        return new BinaryData(*this);
    }

    void* BinaryData::getData() const {
        return _data;
    }

    size_t BinaryData::getSize() const {
        return _size;
    }

    void BinaryData::_init() {
        if (_size == 0) throw std::invalid_argument("BinaryData::_init() called with zero size");
        try {
            _data = new char[_size];
        } catch (const std::bad_alloc&) { throw std::bad_alloc(); }
    }
}  // namespace x