#pragma once

#include "shapes/point.h"
#include "shapes/shape.h"
#include <span>


class dependent_shape {
public:
    virtual void recalculate() = 0;
    virtual std::span<dependent_shape*> dependencies() = 0;

    virtual shape& this_shape() = 0;

    virtual ~dependent_shape() = default;
};



// /Yeah, I know, naming could be clearer, but at least it's
// funnier this way, I hope funnier for you too/

// Represents externally defined point on which other
// dependent_shape`s can be dependent upon.
class dependent_independent_point: public dependent_shape {
public:
    dependent_independent_point(math::vec2 coordinate): point_(coordinate) {}

    shape& this_shape() override { return point_; }
    std::span<dependent_shape *> dependencies() override {
        return {}; // This is the non-dependent point
    }

    void recalculate() override {
        // Since it never depends on anything, don't do anything
    }

    math::vec2& coordinate() { return point_.coordinate; }

private:
    point point_;
};

class dependent_intersection_point: public dependent_shape {
public:
    shape& this_shape() override { return point_; }
    std::span<dependent_shape *> dependencies() override {
        return std::span<dependent_shape*> { intersecting_shapes_, 2 };
    }

    void recalculate() override {
        std::vector<math::vec2> points;

        shape *shapes[] = {
            &intersecting_shapes_[0]->this_shape(),
            &intersecting_shapes_[1]->this_shape(),
        };

        shapes[0]->intersect(*shapes[1], points);
        // TODO: This result should _definitely_ be cached! (!)

        point_.coordinate = points[intersection_id_];
    }

private:
    point point_;

    dependent_shape *intersecting_shapes_[2];

    // Two shapes can have arbitrary many intersections, but this class
    // represents only one intersection point, no more, no less -- just one.

    // Which means that we should somehow distinguish this point from
    // all the other points... which is pretty impossible, but let's say
    // that order in which shape.intersect gives points is predictable,
    // than we should be fine with just storing it's index.

    // NOTE: My other idea is to find closes intersection point to this one
    //       (which should, in theory, give very predictable results
    //        for all possible contiguous movements which the primary use) 
    int intersection_id_;

    dependent_intersection_point(dependent_shape &a, dependent_shape &b, int id):
        intersecting_shapes_ { &a, &b }, point_({}) {
        recalculate(); // TODO:          ^~~~~~ inline init
                       //       would be very nice to do here
    }

    // This is now the way to intersect object while retaining ability to recalculate them later:
    static std::vector<dependent_intersection_point> intersect_all(dependent_shape &a, dependent_shape &b) {
        // TODO: proof of concept horrible copy-pasta (!) (!) (!)

        std::vector<math::vec2> points;
        // TODO: feels like all this vectors everywhere where I want to intersect something are horribly
        //       slow, should try to at least early exit if it's obvious that my objects don't intersect!

        shape *shapes[] = { &a.this_shape(), &b.this_shape() };
        shapes[0]->intersect(*shapes[1], points);
        // TODO: This result should _definitely_ be cached! (!) // even worse now :(

        std::vector<dependent_intersection_point> intersections;
        for (int i = 0; i < points.size(); ++ i) {
            dependent_intersection_point intersection { a, b, i /* <= for all */ };
            intersections.push_back(intersection);
        }

        return intersections;
    }
};




// TODO: WRITE: Finished off here, continue implementing...
class dependent_circle: public dependent_shape {
private:
    dependent_shape *from, *to;

    void recalculate() override {}
    shape& this_shape() override {};
    std::span<dependent_shape *> dependencies() override {}
    
};
