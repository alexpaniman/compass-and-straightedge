#pragma once

#include "drawing-manager.h"
#include "vec.h"

class shape {
public:
    // Tells which shape is it, so its kind of my take on RTTI,
    // which is used for intersection resolution:
    virtual const char* intersection_resolution_name() const noexcept = 0;
    virtual bool intersect(const shape &other, std::vector<math::vec2> &points) const noexcept = 0;

    virtual void draw(gl::drawing_manager mgr) const = 0;

    virtual ~shape() = default;
};

