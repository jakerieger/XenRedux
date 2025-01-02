// Author: Jake Rieger
// Created: 12/31/2024.
//

#include "StateBuffer.hpp"

namespace x {
    void StateBuffer::init(const GameState& initState) {
        for (int i = 0; i < kBufferCount; i++) {
            _buffers[i] = initState.clone();
            _bufferStates[i].store(BufferState::Available, std::memory_order_release);
        }

        _writeIndex.store(0, std::memory_order_release);
        _readIndex.store(0, std::memory_order_release);
    }

    void StateBuffer::cleanup() {
        _isShuttingDown.store(true, std::memory_order_release);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // One frame at 60fps

        for (auto& buffer : _buffers) {
            buffer.releaseAllResources();
        }

        for (auto& state : _bufferStates) {
            state.store(BufferState::Available, std::memory_order_release);
        }

        _writeIndex.store(0, std::memory_order_release);
        _readIndex.store(0, std::memory_order_release);
    }

    GameState& StateBuffer::getWriteBuffer() {
        i32 currentWrite = _writeIndex.load(std::memory_order_acquire);
        _bufferStates[currentWrite].store(BufferState::Writing, std::memory_order_release);
        return _buffers[currentWrite];
    }

    const GameState& StateBuffer::getReadBuffer() const {
        return _buffers[_readIndex.load(std::memory_order_acquire)];
    }

    void StateBuffer::swapWriteBuffer() {
        if (_isShuttingDown.load(std::memory_order_acquire)) { return; }

        i32 currentWrite = _writeIndex.load(std::memory_order_acquire);
        i32 nextIndex    = (currentWrite + 1) % kBufferCount;
        _bufferStates[currentWrite].store(BufferState::Ready, std::memory_order_release);

        BufferState expected = BufferState::Available;
        for (int attempts = 0; attempts < 3; ++attempts) {
            if (_bufferStates[nextIndex].compare_exchange_strong(expected,
                                                                 BufferState::Writing,
                                                                 std::memory_order_acq_rel)) {
                _writeIndex.store(nextIndex, std::memory_order_release);
                return;
            }
            expected = BufferState::Available;
            std::this_thread::yield();
        }
    }

    void StateBuffer::swapReadBuffer() {
        if (_isShuttingDown.load(std::memory_order_acquire)) { return; }

        i32 currentRead = _readIndex.load(std::memory_order_acquire);
        i32 targetWrite = _writeIndex.load(std::memory_order_acquire);

        if (currentRead != targetWrite) {
            if (_bufferStates[targetWrite].load(std::memory_order_acquire) == BufferState::Ready) {
                _bufferStates[currentRead].store(BufferState::Available, std::memory_order_release);
                _bufferStates[targetWrite].store(BufferState::Reading, std::memory_order_release);
                _readIndex.store(targetWrite, std::memory_order_release);
            }
        }
    }
}  // namespace x