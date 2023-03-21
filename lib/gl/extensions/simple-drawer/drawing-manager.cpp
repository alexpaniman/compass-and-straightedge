#include "colored-vertex.h"
#include "math.h"
#include "drawing-manager.h"
#include <iostream>

namespace gl {

    void drawing_manager::set_color(math::vec3 color) {
        m_current_color = { color.r(), color.g(), color.b() };
    }

    void drawing_manager::set_axes(math::axes axes) { m_axes = axes; }

    void drawing_manager::add_axes(math::axes axes) {
        set_axes(axes.apply(m_axes));
    }

    drawing_manager drawing_manager::with_applied(math::axes axes) {
        drawing_manager new_mgr = *this;
        new_mgr.add_axes(axes);

        return new_mgr;
    }

    void drawing_manager::set_width(float width) {
        m_width = width;
    }

    static math::vec2 fix_coordinates(math::vec2 point) { // TODO: improve
        point.y() = - point.y();
        return point;
    }

    void drawing_manager::draw_interpolated_triangle(colored_vertex p0, colored_vertex p1, colored_vertex p2) {
        m_vertices.insert(m_vertices.end(), {
            { fix_coordinates(m_axes.get_view_coordinates(p0.point)), p0.color },
            { fix_coordinates(m_axes.get_view_coordinates(p1.point)), p1.color },
            { fix_coordinates(m_axes.get_view_coordinates(p2.point)), p2.color }
        });
    }

    void drawing_manager::draw_triangle(math::vec2 p0, math::vec2 p1, math::vec2 p2) {
        draw_interpolated_triangle({ p0, m_current_color },
                                   { p1, m_current_color },
                                   { p2, m_current_color });
    }


    void drawing_manager::draw_rectangle(math::vec2 x0, math::vec2 x1) {
        // Maybe, in the future, use index buffer instead:
        draw_triangle(x0, { x0.x(), x1.y() }, x1);
        draw_triangle(x0, { x1.x(), x0.y() }, x1);
    }

    void drawing_manager::draw_line(math::vec2 from, math::vec2 to) {
        math::vec direction = from - to;
        math::vec shift = direction
            .perpendicular().normalized();

        // Line will be separated in 1/4 (2/4 for monochrome middle,
        // and 1/2 for antialiased halfs):
        shift *= (m_width / 2.0f);

        math::vec cap_shift = direction.normalized();
        cap_shift.normalize() *= m_width / 2.0f;

        // Apply rectangulare cap to the line:
        from += cap_shift, to -= cap_shift;

        // Vertices of rectangle
        math::vec p0 = from, p1 = from, p2 = to, p3 = to;

        // ==> Draw monochrome middle:

        p1 -= shift, p3 -= shift;
        p0 += shift, p2 += shift;

        draw_triangle(p0, p1, p2), draw_triangle(p3, p1, p2);
    }

    void drawing_manager::draw_antialiased_line(math::vec2 from, math::vec2 to,
                                                float antialiasing_level) {
        // Now antialiasing is enabled in OpenGL itself,
        // fallback to drawing "simple" line:

        #ifndef GL_NO_BUILTIN_ANTIALIASING

        draw_line(from, to);
        (void) antialiasing_level;

        #else

        math::vec direction = from - to;
        math::vec shift = direction
            .perpendicular().normalized();

        // Line will be separated in 1/4 (2/4 for monochrome middle,
        // and 1/2 for antialiased halfs):
        shift *= (m_width / 4.0f);

        math::vec cap_shift = direction.normalized();
        cap_shift.normalize() *= m_width / 2.0f;

        // Apply rectangulare cap to the line:
        from += cap_shift, to -= cap_shift;

        // Vertices of rectangle
        math::vec p0 = from, p1 = from, p2 = to, p3 = to;

        // ==> Draw monochrome middle:

        p1 -= shift, p3 -= shift;
        p0 += shift, p2 += shift;

        draw_triangle(p0, p1, p2), draw_triangle(p3, p1, p2);

        // Draw left half:
        math::vec desaturated_color = m_current_color;

        // Should be completely transparent:
        desaturated_color.a() = 1.0f - antialiasing_level;

        draw_interpolated_triangle({ p0 + shift,      desaturated_color },
                                   { p2 + shift,      desaturated_color },
                                   { p1 + shift * 2,    m_current_color });

        draw_interpolated_triangle({ p2 + shift,     desaturated_color },
                                   { p1 + shift * 2,   m_current_color },
                                   { p3 + shift * 2,   m_current_color });


        draw_interpolated_triangle({ p1 - shift,     desaturated_color },
                                   { p3 - shift,     desaturated_color },
                                   { p0 - shift * 2,   m_current_color });

        draw_interpolated_triangle({ p3 - shift,     desaturated_color },
                                   { p2 - shift * 2,   m_current_color },
                                   { p0 - shift * 2,   m_current_color });

        #endif
    }

    static math::vec2 rot(math::vec2 current, float angle) {
        return { (float) cos(angle) * current.x() - (float) sin(angle) * current.y(),
                 (float) sin(angle) * current.x() + (float) cos(angle) * current.y() };
    }

    void drawing_manager::draw_vector(math::vec2 from, math::vec2 to) {
        math::vec l = rot(from - to, -0.4f).normalized() * 0.1f;
        math::vec r = rot(from - to, +0.4f).normalized() * 0.1f;

        draw_antialiased_line(from, to - (to - from).normalized() * 0.04f);
        draw_triangle(to, to + l, to + r);
    }

}
