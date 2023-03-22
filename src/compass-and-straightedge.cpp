#include "circle.h"
#include "drawing-manager.h"
#include "line.h"

#include "gl-imgui.h"
#include "ui-window.h"
#include "vec.h"
#include <memory>

using namespace math;

class compass_and_straightedge: public gl::imgui::ui_window {
public:
    using gl::imgui::ui_window::ui_window;

    void init() override {
        //  shapes_.emplace_back(new circle({ 0.0f, 0.0f }, 0.3f));
        // shapes_.emplace_back(new circle({ 0.2f, 0.1f }, 0.2f));
        shapes_.emplace_back(new line({ -0.2f, -0.1f }, { 0.2f, 0.2f }));
        shapes_.emplace_back(new line({ 0.4f, 0.1f }, { 0.2f, 0.4f }));
    }


    void draw_ui() override {
        ImGui::Begin("List of shapes");

        for (const auto& colored_shape: { "Hello", "World", "My", "Attack on Titan" })
            ImGui::Selectable(colored_shape);

        ImGui::End();
    }

    // mgr.set_color({ 1.0f, 0.2f, 0.5f });
    void draw_main(gl::drawing_manager mgr) override {
        // draw background:
        mgr.set_color({ 0.74f, 0.63f, 0.72f });
        mgr.draw_rectangle({ -1.0f, -1.0f }, { 1.0f, 1.0f });

        // draw shapes:
        mgr.set_color({ 0.7f, 0.9f, 0.6f });
        mgr.set_width(0.007f);

        draw_shapes(mgr);

        // draw their intersections:
        draw_intersections(mgr);
    }

private:
    std::vector<std::unique_ptr<shape>> shapes_;

    void draw_point(gl::drawing_manager mgr, math::vec2 point) {
        constexpr double outer_point_size = 0.01;
        constexpr double inner_point_size = 0.005;

        mgr.set_color({ 0.f, 0.f, 0.f }); // outer color
        circle(point, outer_point_size).fill(mgr);

        mgr.set_color({ 1.f, 1.f, 1.f }); // inner color
        circle(point, inner_point_size).fill(mgr); 
    }

    void draw_intersections(gl::drawing_manager mgr) {
        std::vector<math::vec2> intersection_points;
        for (std::size_t i = 0; i < shapes_.size(); ++ i)
            for (std::size_t j = i + 1; j < shapes_.size(); ++ j)
                shapes_[i]->intersect(*shapes_[j], intersection_points);

        for (auto &&point: intersection_points)
            draw_point(mgr, point);
    }

    void draw_shapes(gl::drawing_manager mgr) {
        for (auto &&shape: shapes_)
            shape->draw(mgr);
    }
};

int main() {
    compass_and_straightedge drawer(1080, 1080, "Compass and Straightedge | OpenGL");
    drawer.draw_loop();
}
 
