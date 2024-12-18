// Author: Jake Rieger
// Created: 12/13/2024.
//

#pragma once
#include "Types.hpp"

#include <mutex>

namespace x {
    /// @brief Generic data interface for objects which contain data.
    /// Design heavily inspired by Love2d's "Data" class
    class IData {
    public:
        IData()          = default;
        virtual ~IData() = default;

        virtual IData* clone() const   = 0;
        virtual void* getData() const  = 0;
        virtual size_t getSize() const = 0;

        std::mutex* getMutex();

    private:
        std::mutex _mutex;
        std::once_flag _created;
    };
}  // namespace x
