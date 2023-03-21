#pragma once

#include <cstddef>
#include <type_traits>

namespace math {

    template <typename type>
    constexpr type pow(type value, size_t power) {
        if (power == 1)
            return value;

        return power % 2 == 0?
              pow(value * value, power / 2)
            : value * pow(value, power - 1);
    }

    template <typename type>
    constexpr type clamp(type value, std::type_identity_t<type> min, std::type_identity_t<type> max) {
        if (value < min)
            return min;

        if (value > max)
            return max;

        return value;
    }

    template <typename type>
    constexpr bool in_range(type value, std::type_identity_t<type> min, std::type_identity_t<type> max) {
        return max >= value && value >= min;
    }

}
