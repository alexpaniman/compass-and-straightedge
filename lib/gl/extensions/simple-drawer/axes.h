#pragma once

#include "vec.h"
#include "rect.h"

namespace math {

    class axes {
    public:
        axes(rectangle world, rectangle view)
            : m_world(world), m_view(view) {}

        axes(rectangle view)
            : axes(rectangle { { -1.0f, -1.0f }, { 1.0f, 1.0f } }, view) {}

        axes(math::vec2 x0, math::vec2 x1):
            axes(rectangle { x0, x1 }) {}

        axes(): axes(math::vec2 { -1.0f, -1.0f }, math::vec2 { 1.0f, 1.0f }) {}

        axes apply(axes other_axes) {
            return { m_world, math::rectangle { other_axes.get_view_coordinates(m_view.x0),
                                                other_axes.get_view_coordinates(m_view.x1) } };
        }

        math::vec2 get_view_coordinates(math::vec2 source) const {
            using math::vec;

            // Position of original coordinate centers
            vec dst_shift = {  m_view.x0.x(),  m_view.x1.y() };
            vec src_shift = { m_world.x0.x(), m_world.x1.y() };

            source -= src_shift; // Undo source shift

            vec ratio = vec {  m_view.x1.x(),  m_view.x0.y() } - dst_shift;
            ratio    /= vec { m_world.x1.x(), m_world.x0.y() } - src_shift;

            return (ratio *= source) += dst_shift;
            //            ^~ Convert to target basis
        }

        math::vec2 get_world_coordinates(math::vec2 source) const {
            return axes(m_view, m_world).get_view_coordinates(source);
        }

        rectangle m_world, m_view;
    };

}
