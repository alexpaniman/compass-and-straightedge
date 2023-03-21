#pragma once

#include "vec.h"

struct colored_vertex final {
    math::vec2 point;
    math::vec3 color;

    colored_vertex()
        : point(0.0f, 0.0f), color(0.0f, 0.0f, 0.0f) {};

    colored_vertex(math::vec2 new_point,
                   math::vec3 new_color)
        : point(new_point), color(new_color) {};
};
