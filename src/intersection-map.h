#pragma once

#include "shape.h"

#include <algorithm>
#include <compare>
#include <cstring>
#include <initializer_list>
#include <map>

// Helper class that simplifies writing intersectors for different shapes,
// basically it's an implementation of a double-dispatch resolved by
// shape::intersection_resolution_name()

// It also uses convenient specialty of shape intersections: it's commutative,
// so you only ever need to implement it one way, and you will get the
// reverse for free and automatically.

// Beware if your class doesn't implement intersectors for all other shapes
// you can get a stack overflow during intersection probing, there's no way 
// of detecting this error cheaply and conveniently in the code.

template <typename derived_shape>
class intersection_map {
private:
    using intersector = bool (const derived_shape&, const shape&, std::vector<math::vec2> &points);

public:
    class marked_intersection_resolver {
    private:
        template <typename other_shape>
        using member_intersector =
            bool (derived_shape::*) (const other_shape&, std::vector<math::vec2> &points) const;

    public:
        template <typename other_shape>
        marked_intersection_resolver(const char *target_shape_name, member_intersector<other_shape> intersector):
            target_shape_name(target_shape_name),
            intersect([=](const derived_shape &current, const shape& other,
                          std::vector<math::vec2> &points) {
                return (current.*intersector)(static_cast<const other_shape&>(other), points);
            }) {}

        const char* target_shape_name;
        std::function<intersector> intersect;
    };

    intersection_map(std::initializer_list<marked_intersection_resolver> resolvers) {
        for (auto &[name, resolver]: resolvers)
            intersection_resolvers_.insert({ name, resolver });
    }

    bool intersect(const derived_shape &current, const shape &other, std::vector<math::vec2> &points) {
        std::string figure_name = other.intersection_resolution_name();

        auto selected_marked_resolver = intersection_resolvers_.find(figure_name);
        if (selected_marked_resolver == intersection_resolvers_.end())
            return other.intersect(current, points); // fallback to other intersector

        auto [target_shape_name, intersect] = *selected_marked_resolver;
        return intersect(current, other, points);
    }

private:
    std::map<std::string, std::function<intersector>> intersection_resolvers_;
};
