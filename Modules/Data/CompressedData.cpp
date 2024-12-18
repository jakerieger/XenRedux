// Author: Jake Rieger
// Created: 12/13/2024.
//

#include "CompressedData.hpp"

namespace x {
    CompressedData::CompressedData(const void* data,
                                   size_t size,
                                   size_t originalSize,
                                   CompressionType compressionType)
        : _data(nullptr), _size(size), _originalSize(originalSize), _type(compressionType) {
        _init();
        if (data != nullptr) memcpy(_data, data, _size);
    }

    CompressedData::CompressedData(const std::vector<u8>& data,
                                   size_t originalSize,
                                   CompressionType compressionType)
        : _data(nullptr), _size(data.size()), _originalSize(originalSize), _type(compressionType) {
        _init();
        memcpy(_data, data.data(), data.size());
    }

    CompressedData::CompressedData(const CompressedData& data)
        : _data(nullptr), _size(data._size), _originalSize(data._originalSize), _type(data._type) {
        _init();
        memcpy(_data, data._data, data._size);
    }

    CompressedData::~CompressedData() {
        delete[] _data;
    }

    CompressedData* CompressedData::clone() const {
        return new CompressedData(*this);
    }

    void* CompressedData::getData() const {
        return _data;
    }

    size_t CompressedData::getSize() const {
        return _size;
    }

    size_t CompressedData::getOriginalSize() const {
        return _originalSize;
    }

    CompressionType CompressedData::getCompressionType() const {
        return _type;
    }

    void CompressedData::_init() {
        if (_size == 0)
            throw std::invalid_argument("CompressedData::_init() called with zero size");
        try {
            _data = new char[_size];
        } catch (const std::bad_alloc&) { throw std::bad_alloc(); }
    }
}  // namespace x