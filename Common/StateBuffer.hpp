// Author: Jake Rieger
// Created: 12/31/2024.
//

#pragma once

#include "Types.hpp"
#include "GameState.hpp"

#include <array>
#include <atomic>
#include <mutex>

namespace x {
    class StateBuffer {
    public:
        void init(const GameState& initState);
        void cleanup();

        GameState& getWriteBuffer();
        const GameState& getReadBuffer() const;

        void swapWriteBuffer();
        void swapReadBuffer();

    private:
        enum class BufferState { Available, Writing, Ready, Reading };
        static constexpr i32 kBufferCount = 3;

        std::array<GameState, kBufferCount> _buffers;
        std::array<std::atomic<BufferState>, kBufferCount> _bufferStates;
        std::atomic<i32> _writeIndex = {0};
        std::atomic<i32> _readIndex  = {0};
        std::atomic<bool> _isShuttingDown {false};
    };
}  // namespace x
