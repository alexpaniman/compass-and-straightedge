#pragma once

#include "drawing-manager.h"
#include "vec.h"

class shape {
public:
    // Tells which shape is it, so its kind of my take on RTTI,
    // which is used for intersection resolution:
    virtual const char* intersection_resolution_name() const noexcept = 0;
    virtual bool intersect(const shape &other, std::vector<math::vec2> &points) const noexcept = 0;

    virtual math::vec2 project(math::vec2 point) const noexcept = 0;
    float distance(math::vec2 point) const { return (project(point) - point).len(); }

    virtual void draw(gl::drawing_manager mgr) const = 0;

    virtual ~shape() = default;
};

