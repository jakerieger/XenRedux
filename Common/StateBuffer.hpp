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

        // Called by update thread when it's done writing
        void swapWriteBuffer();

        // Called by render thread when it's ready to render
        void swapReadBuffer();

    private:
        static constexpr i32 kBufferCount = 3;  // Triple buffering
        std::array<GameState, kBufferCount> _buffers;
        std::atomic<i32> _writeIndex = {0};
        std::atomic<i32> _readIndex  = {0};
        std::mutex _swapMutex;
    };
}  // namespace x
