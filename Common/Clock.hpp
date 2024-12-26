// Author: Jake Rieger
// Created: 12/23/2024.
//

#pragma once

#include "Types.hpp"
#include <chrono>
#include <memory>

namespace x {
    class Clock {
    public:
        Clock() = default;

        static std::shared_ptr<Clock> create();

        /// @brief Returns frame delta time in seconds.
        [[nodiscard]] f32 getDeltaTime() const;

        /// @brief Returns frame time in millis.
        [[nodiscard]] f32 getFrameTime() const;

        /// @brief Returns the elapsed time between now and the clock starting.
        [[nodiscard]] i64 getElapsedTime() const;

        /// @brief Returns the frame rate in frames per second.
        [[nodiscard]] f32 getFrameRate() const;

        /// @brief Starts the clock.
        void start();

        /// @brief Stops the clock.
        void stop();

        /// @brief Tick frame counter one. Updates delta time and capture the start time of the
        /// frame.
        /// @note This should be called at the beginning of your frame.
        void tick();

        /// @brief Captures the end time of the frame and updates frame time.
        /// @note This should be called at the end of your frame.
        void update();

        /// @brief Reads the value of the CPU's time-stamp counter.
        static u64 cpuTimestamp();

    private:
#if defined(WIN32) || defined(WIN64)
        using ClockTime = std::chrono::time_point<std::chrono::steady_clock>;
#else
        using ClockTime = std::chrono::time_point<std::chrono::system_clock>;
#endif

        bool _running  = false;
        f32 _deltaTime = 0;
        f32 _frameTime = 0;
        ClockTime _lastTime;
        ClockTime _startTime;
    };
}  // namespace x
