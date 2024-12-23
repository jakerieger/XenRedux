// Author: Jake Rieger
// Created: 12/23/2024.
//

#include "Clock.hpp"

#if defined(WIN32) || defined(WIN64)
    #include <intrin.h>
#endif

namespace x {
    f32 Clock::getDeltaTime() const {
        return _deltaTime;
    }

    f32 Clock::getFrameTime() const {
        return _frameTime;
    }

    i64 Clock::getElapsedTime() const {
        if (_running) {
            const auto now     = std::chrono::high_resolution_clock::now();
            const auto elapsed = now - _startTime;
            return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(_lastTime - _startTime)
          .count();
    }

    f32 Clock::getFrameRate() const {
        return 1.f / getDeltaTime();
    }

    void Clock::start() {
        _running   = true;
        _startTime = std::chrono::high_resolution_clock::now();
        _lastTime  = _startTime;
    }

    void Clock::stop() {
        _running = false;
    }

    void Clock::tick() {
        const auto now                             = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<f32> deltaTime = now - _lastTime;
        _deltaTime                                 = deltaTime.count();
        _lastTime                                  = now;
    }

    void Clock::update() {
        const auto frameEnd                            = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<f32> frameDuration = frameEnd - _lastTime;
        _frameTime                                     = frameDuration.count();
    }

    u64 Clock::cpuTimestamp() {
#ifdef _MSC_VER
        return __rdtsc();
#elif defined(__clang__) || defined(__GNUC__)
        u32 lo, hi;
        __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
        return ((u64)hi << 32) | lo;
#else
    #error "Unsupported compiler"
#endif
    }
}  // namespace x