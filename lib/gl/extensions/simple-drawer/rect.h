#pragma once

#include "math-utils.h"
#include "vec.h"

namespace math {

    struct rectangle {
        math::vec2 x0, x1; // Top left point and bottom right point

        explicit rectangle(math::vec2 top_left, math::vec2 bottom_right)
            : x0(top_left), x1(bottom_right) {}

        rectangle& shrink(math::vec2 delta) {
            x0 += delta;
            x1 -= delta;

            return *this;
        }

        rectangle& absorb(const math::rectangle other) {
            x0.x() = std::min((float) x0.x(), other.x0.x());
            x0.y() = std::min((float) x0.y(), other.x0.y());

            x1.x() = std::max((float) x1.x(), other.x1.x());
            x1.y() = std::max((float) x1.y(), other.x1.y());

            return *this;
        }

        bool contains(math::vec2 position) {
            return math::in_range(position.x(), x0.x(), x1.x())
                && math::in_range(position.y(), x0.y(), x1.y());
        }
    };

}
