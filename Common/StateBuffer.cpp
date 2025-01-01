// Author: Jake Rieger
// Created: 12/31/2024.
//

#include "StateBuffer.hpp"

namespace x {
    void StateBuffer::init(const GameState& initState) {
        std::lock_guard<std::mutex> lock(_swapMutex);
        // Clone init state
        for (int i = 0; i < kBufferCount; i++) {
            _buffers[i] = initState.clone();
        }
        _writeIndex.store(0);
        _readIndex.store(0);
    }

    void StateBuffer::cleanup() {
        std::lock_guard<std::mutex> lock(_swapMutex);
        for (auto& buffer : _buffers) {
            buffer.releaseAllResources();
        }
        _buffers = std::array<GameState, kBufferCount>();
        _writeIndex.store(0);
        _readIndex.store(0);
    }

    GameState& StateBuffer::getWriteBuffer() {
        return _buffers[_writeIndex];
    }

    const GameState& StateBuffer::getReadBuffer() const {
        return _buffers[_readIndex];
    }

    void StateBuffer::swapWriteBuffer() {
        std::lock_guard<std::mutex> lock(_swapMutex);
        i32 nextIndex = (_writeIndex.load() + 1) % kBufferCount;
        // Only swap if the next buffer isn't being read
        if (nextIndex != _readIndex.load()) { _writeIndex.store(nextIndex); }
    }

    void StateBuffer::swapReadBuffer() {
        std::lock_guard<std::mutex> lock(_swapMutex);
        if (_readIndex.load() != _writeIndex.load()) { _readIndex.store(_writeIndex.load()); }
    }
}  // namespace x