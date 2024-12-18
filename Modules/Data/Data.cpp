// Author: Jake Rieger
// Created: 12/13/2024.
//

#include "Data.hpp"

namespace x {
    std::mutex* IData::getMutex() {
        return &_mutex;
    }
}  // namespace x