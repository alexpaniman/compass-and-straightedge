#pragma once

#include "intersection-map.h"
#include "shape.h"
#include "vec.h"

class circle: public shape {
public:
    math::vec2 center; // circle is defined by its center and radius, this are public as
    float radius;      // shapes are considered to be just data you can use however you want!

    circle(math::vec2 center, float radius): center(center), radius(radius) {}
    circle(math::vec2 center, math::vec2 side): 
        circle(center, (side - center).len()) {}


    const char* intersection_resolution_name() const noexcept override { return "circle"; }

    bool intersect_circle(const circle &other, std::vector<math::vec2> &points) const noexcept;
    bool intersect(const shape &other, std::vector<math::vec2> &points) const noexcept override {
        static intersection_map<circle> intersection_map {
            { "circle", &circle::intersect_circle }
        };

        return intersection_map.intersect(*this, other, points);
    }


    math::vec2 project(math::vec2 point) const noexcept override;


    void draw(gl::drawing_manager mgr) const override;
    void fill(gl::drawing_manager mgr) const;
};
