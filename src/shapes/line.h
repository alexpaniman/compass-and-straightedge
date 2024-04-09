#pragma once

#include "intersection-map.h"
#include "circle.h"
#include "shape.h"
#include "vec.h"

class line: public shape {
public:
    math::vec2 x0, x1; // line is defined by two points somewhere on the line, this are public
                       // as shapes are considered to be just data you can use however you want!

    line(math::vec2 x0, math::vec2 x1): x0(x0), x1(x1) {}


    const char* intersection_resolution_name() const noexcept override { return "line"; }

    bool intersect_line(const line& line,       std::vector<math::vec2> &points) const;
    bool intersect_circle(const circle& circle, std::vector<math::vec2> &points) const;
    bool intersect(const shape &other, std::vector<math::vec2> &points) const noexcept override {
        static intersection_map<line> intersection_map {
            { "circle", &line::intersect_circle },
            { "line"  , &line::intersect_line   }
        };

        return intersection_map.intersect(*this, other, points);
    }


    math::vec2 project(math::vec2 point) const noexcept override;


    void draw(gl::drawing_manager mgr) const override;
};
