// Author: Jake Rieger
// Created: 12/26/2024.
//

#pragma once

#include <random>

namespace x::Math::Random {
    template<class T>
    concept Numeric = std::is_arithmetic_v<T>;

    template<class T>
    struct DistributionSelector {
        using type = std::conditional_t<std::is_integral_v<T>,
                                        std::uniform_int_distribution<T>,
                                        std::uniform_real_distribution<T>>;
    };

    template<Numeric T>
    T getRandomRange(T min, T max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        typename DistributionSelector<T>::type distrib(min, max);
        return distrib(gen);
    }
}  // namespace x::Math::Random