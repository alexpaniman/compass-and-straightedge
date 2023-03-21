#pragma once

#include "axes.h"
#include "colored-vertex.h"
#include "opengl-setup.h"
#include "vertex-vector-array.h"
#include "vec.h"

namespace gl {

    class drawing_manager {
    public:
        // Uses black color by default
        drawing_manager(gl::vertex_vector_array<colored_vertex>& vertices)
            : m_vertices(vertices), m_current_color(0.0f, 0.0f, 0.0f) {}

        // ==> Control current settings:

        void set_color(math::vec3 color);

        void set_width(float width);
        void set_axes(math::axes axes);
        void add_axes(math::axes axes);

        drawing_manager with_applied(math::axes axes);

        // ==> Draw shapes:

        void draw_interpolated_triangle(colored_vertex p0, colored_vertex p1, colored_vertex p2);

        void draw_triangle(math::vec2 p0, math::vec2 p1, math::vec2 p2);

        void draw_rectangle(math::vec2 x0, math::vec2 x1);

        void draw_line(math::vec2 from, math::vec2 to);
        void draw_antialiased_line(math::vec2 from, math::vec2 to,
                                   float antialiasing_level = 0.8f);

        void draw_vector(math::vec2 from, math::vec2 to);

    private:
        gl::vertex_vector_array<colored_vertex>& m_vertices;

    public:
        // ==> Current settings:
        math::axes m_axes;

    private:
        math::vec3 m_current_color;
        float m_width;
    };

}
