#pragma once

#include "intersection-map.h"
#include "shape.h"

#include "circle.h"
#include "line.h"


// TODO: make .cpp file for this!

class point: public shape {
public:
    math::vec2 coordinate;

    point(math::vec2 coordinate): coordinate(coordinate) {}

    const char* intersection_resolution_name() const noexcept override { return "point"; }

    bool intersect(const shape &other, std::vector<math::vec2> &points) const noexcept override {
        // TODO: make this constant configurable externally (probably) 
        constexpr double trigger_multiplier = 1.5f;
        const double outer_point_size = 0.01; // TODO: copy-pasted in point::draw!!

        return other.distance(coordinate) < trigger_multiplier * outer_point_size;
    }

    math::vec2 project(math::vec2 point) const noexcept override { return coordinate; }

    void draw(gl::drawing_manager mgr) const override {
        // TODO: this constants need to be configurable externally
        const math::vec3 background_color = { 0.13f, 0.11f, 0.12f };
        const double outer_point_size = 0.01;
        const double inner_point_size = 0.007;

        circle(coordinate, outer_point_size).fill(mgr);

        mgr.set_outer_color(mgr.fill_color());
        circle(coordinate, inner_point_size).fill(mgr); 
    }
};
