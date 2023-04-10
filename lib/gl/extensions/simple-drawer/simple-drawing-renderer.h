#pragma once

#include "drawing-manager.h"
#include "opengl-setup.h"
#include "renderer.h"
#include "vec-layout.h"

namespace gl {

    template <typename rendering_function>
    class simple_drawing_renderer: public gl::renderer {
    public:
        simple_drawing_renderer(rendering_function draw)
            : m_draw(draw) {}

        void setup() override final {
            m_gradient_shader.from_file("res/gradient.glsl");
            m_verticies.set_layout(math::vector_layout<float, 2>() +
                                   math::vector_layout<float, 3>());
        }

        void draw()  override final {
            m_verticies.clear();

            drawing_manager draw_mgr = drawing_manager { m_verticies }.with_applied(transformer);
            m_draw(draw_mgr);

            m_verticies.update();
            gl::draw(gl::drawing_type::TRIANGLES, m_verticies, m_gradient_shader);
        }

        // TODO: make private
        math::axes transformer = math::rectangle { { -1.f, -1.f }, { 1.f, 1.f } };

    private:
        gl::shaders::shader_program m_gradient_shader;
        gl::vertex_vector_array<colored_vertex> m_verticies;

        rendering_function m_draw;
    };

    // TODO: exactly the same as class above, but for inheritance, extract!
    class vector_renderer: public gl::renderer {
    public:
        vector_renderer() = default;

        void setup() override final {
            m_gradient_shader.from_file("res/gradient.glsl");
            m_verticies.set_layout(math::vector_layout<float, 2>() +
                                   math::vector_layout<float, 3>());
        }

        void draw()  override final {
            m_verticies.clear();

            drawing_manager draw_mgr { m_verticies };
            vector_draw(draw_mgr);

            m_verticies.update();
            gl::draw(gl::drawing_type::TRIANGLES, m_verticies, m_gradient_shader);
        }

        virtual void vector_draw(gl::drawing_manager &mgr) = 0;

    private:
        gl::shaders::shader_program m_gradient_shader;
        gl::vertex_vector_array<colored_vertex> m_verticies;
    };

}
