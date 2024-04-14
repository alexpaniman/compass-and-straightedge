#include "dependent-shapes/dependent-shape.h"
#include "gl-imgui.h"

#include "shapes/line.h"
#include "shapes/circle.h"

#include "icons-font-awesome-5.h"

#include <memory>

using namespace math;

class compass_and_straightedge: public gl::imgui::ui_window {
public:
    using gl::imgui::ui_window::ui_window;

    // configure current window:
    int desired_fps() override { return 60; }
    bool should_redraw_in_loop() override { return true; }


    void draw_ui() override {
    }

    void draw_main(gl::drawing_manager mgr) override {
        // ==> draw background:
        mgr.set_outer_color(background_color);
        mgr.draw_rectangle(mgr.screen());


        for (auto &&shape: shapes_)
            shape->this_shape().draw(mgr);


    }

    void on_mouse_moved(math::vec2 cursor) override {

    }

    void on_mouse_pressed(math::vec2 cursor, gl::mouse_button button, gl::action action) override {
        if (button != gl::mouse_button::LEFT || action != gl::action::PRESS)
            return;

        shapes_.emplace_back(new dependent_independent_point(cursor));

    }

private:
    std::vector<std::unique_ptr<dependent_shape>> shapes_;



private:
    //          ==> general look configuration <==

    // ==> canvas background color:
    math::vec3       background_color = { 0.13f, 0.11f, 0.12f };

    // ==> default shape colors
    math::vec3           circle_color = { 0.67f, 0.60f, 0.82f };

    math::vec3       inner_line_color = { 0.90f, 0.20f, 0.30f };
    math::vec3       outer_line_color = { 0.99f, 0.59f, 0.39f };

    math::vec3            point_color = { 0.65f, 0.85f, 0.46f };

    // ==> selection colors
    math::vec3   selected_shape_color = { 0.20f, 0.70f, 0.60f };

    math::vec3   selected_point_color = { 0.90f, 0.20f, 0.30f };
    math::vec3    hovered_point_color = { 0.30f, 0.20f, 0.90f };

    math::vec3 new_intersection_color = { 0.46f, 0.86f, 0.91f };
    
    // ==> object and point width:
    double         shape_stroke_width = 0.007;
};

int main() {
    compass_and_straightedge drawer(1080, 1080, "Compass and Straightedge | OpenGL");
    drawer.draw_loop();
}
 
