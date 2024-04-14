#include "drawing-manager.h"
#include "gl-imgui.h"

#include "imgui.h"
#include "line.h"
#include "circle.h"

#include "icons-font-awesome-5.h"
#include "opengl-setup.h"

#include <memory>

using namespace math;

class compass_and_straightedge: public gl::imgui::ui_window {
public:
    using gl::imgui::ui_window::ui_window;

    // configure current window:
    int desired_fps() override { return 60; }
    bool should_redraw_in_loop() override { return true; }


    void draw_ui() override {
        ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoDecoration);

        int* active_tool_ptr = reinterpret_cast<int*>(&active_tool_);
        ImGui::RadioButton(ICON_FA_TIMES_CIRCLE     " Point"  , active_tool_ptr, poke_point);
        ImGui::RadioButton(ICON_FA_DRAFTING_COMPASS " Compass", active_tool_ptr, compass);
        ImGui::RadioButton(ICON_FA_RULER            " Line"   , active_tool_ptr, ruler);

        ImGui::End();
    }

    void draw_main(gl::drawing_manager mgr) override {
        // ==> draw background:
        mgr.set_outer_color(background_color);
        mgr.draw_rectangle(mgr.screen());

        draw_shapes(mgr);

        if (auto cursor = get_cursor_position())
            draw_shape_with_intersections(mgr, *cursor);

        // ==> draw persistent points:
        draw_points(mgr);
    }

    void on_mouse_moved(math::vec2 cursor) override {
        // ==> distance compared to thickness of object to think that it's selected:
        constexpr double trigger_multiplier = 1.5f;

        hovered_point_ = -1;

        for (int i = points_.size() - 1; i >= 0; -- i)
            if ((points_[i] - cursor).len() <= trigger_multiplier * outer_point_size) {
                hovered_point_ = i; // select button when hovered over
                return;
            }

        selected_shape_ = -1;

        if (active_tool_ != poke_point)
            return;

        for (int i = shapes_.size() - 1; i >= 0; -- i)
            if (shapes_[i]->distance(cursor) <= trigger_multiplier * shape_stroke_width) {
                selected_shape_ = i;
                return;
            }
    }

    void on_mouse_pressed(math::vec2 cursor, gl::mouse_button button, gl::action action) override {
        if (button == gl::mouse_button::RIGHT && active_tool_ == compass
            && hovered_point_ != -1 && selected_point_ != -1) {

            selected_radius_ = (points_[hovered_point_] - points_[selected_point_]).len();
            selected_point_  = -1;
        }

        if (button == gl::mouse_button::RIGHT && active_tool_ == ruler
            && hovered_point_ == -1 && selected_point_ != -1) {

            vec2 starting_point = points_[selected_point_];
            shapes_.emplace_back(new line(starting_point, starting_point + vec2 { 0.f, -1e5f }));

            selected_point_ = -1;
        }

        if (button != gl::mouse_button::LEFT)
            return;

        if (action != gl::action::PRESS)
            return;

        if (active_tool_ == poke_point) {
            if (selected_shape_ != -1)
                points_.emplace_back(shapes_[selected_shape_]->project(cursor));
            else
                points_.emplace_back(cursor);

            return;
        }

        if (selected_point_ != -1 && hovered_point_ != -1) {
            add_shape(points_[hovered_point_]);
            selected_point_ = hovered_point_ = -1;
            return;
        }

        selected_point_ = hovered_point_;
        if (selected_radius_ != -1) {
            shapes_.emplace_back(new circle(points_[selected_point_], selected_radius_));
            selected_point_ = -1;
            selected_radius_ = -1;
        }
    }

private:
    // view of all available shapes:
    std::vector<std::unique_ptr<shape>> shapes_;
    std::vector<math::vec2> points_;

    int selected_point_ = -1;
    int  hovered_point_ = -1;

    int selected_shape_ = -1;
    float selected_radius_ = -1;

    enum: int { poke_point = 0, compass = 1, ruler = 2 }
        active_tool_ = poke_point;


    // draws active_tool_ shape from selected_point_
    void add_shape(math::vec2 to) {
        if (active_tool_ == poke_point || selected_point_ == -1)
            return;

        math::vec2 from = points_[selected_point_];

        if (active_tool_ == compass)
            shapes_.emplace_back(new circle(from, to));
        else if (active_tool_ == ruler)
            shapes_.emplace_back(new line(from, to));
        else {
            assert(false && "some shape is not handeled");
        }

        for (std::size_t i = 0; i < shapes_.size() - 1; ++ i)
            shapes_.back()->intersect(*shapes_[i], points_);
    }

    void draw_shape(gl::drawing_manager mgr, const shape &target, bool override_colors = false) {
        const char *shape_name = target.intersection_resolution_name();

        mgr.set_width(shape_stroke_width);

        if (!override_colors) {
            using namespace std::string_literals;

            /*---*/if ("circle"s == shape_name) {
                mgr.set_outer_color(circle_color);
            } else if (  "line"s == shape_name) {
                mgr.set_outer_color(outer_line_color);
                mgr.set_inner_color(inner_line_color);
            } else {
                assert(false && "unhandled shape type");
            }
        }

        target.draw(mgr);
    }

    void draw_shape_with_intersections(gl::drawing_manager mgr, math::vec2 to) {
        if (active_tool_ == poke_point || selected_point_ == -1)
            return;

        math::vec2 from = points_[selected_point_];

        circle target_circle(from, to);
        line   target_line  (from, to);

        const shape *new_shape = nullptr;

        /*---*/if (active_tool_ == compass) {
            new_shape = &target_circle;
        } else if (active_tool_ ==   ruler) {
            new_shape = &target_line;
        } else {
            assert(false && "unhandled shape type");
        }
        
        std::vector<math::vec2> intersection_points;
        for (auto &&shape: shapes_)
            shape->intersect(*new_shape, intersection_points);

        draw_shape(mgr, *new_shape);

        mgr.set_outer_color(new_intersection_color);
        for (auto &&point: intersection_points)
            draw_point(mgr, point);
    }

    void draw_point(gl::drawing_manager mgr, math::vec2 point) {
        circle(point, outer_point_size).fill(mgr);

        mgr.set_outer_color(background_color);
        circle(point, inner_point_size).fill(mgr); 
    }

    void draw_points(gl::drawing_manager mgr) {
        for (int i = 0; i < static_cast<int>(points_.size()); ++ i) {
            if (selected_point_ == i)
                mgr.set_outer_color(selected_point_color);
            else if (hovered_point_ == i)
                mgr.set_outer_color(hovered_point_color);
            else
                mgr.set_outer_color(point_color);

            draw_point(mgr, points_[i]);
        }
        
    }

    void draw_shapes(gl::drawing_manager mgr) {
        for (int i = 0; i < shapes_.size(); ++ i) {
            const shape &current_shape = *shapes_[i];

            // ==> draw selected shape if it's selected:
            if (selected_shape_ == i) {
                mgr.set_outer_color(selected_shape_color);
                mgr.set_inner_color(selected_shape_color);

                draw_shape(mgr, current_shape, /* override colors */ true);
                continue;
            }

            draw_shape(mgr, current_shape);
        }
    }

private:
    //          ==> general look configuration <==

    // ==> canvas background color:
    math::vec3 background_color       = { 0.13f, 0.11f, 0.12f };

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

    double           outer_point_size = 0.01;
    double           inner_point_size = 0.007;
};

int main() {
    compass_and_straightedge drawer(1080, 1080, "Compass and Straightedge | OpenGL");
    drawer.draw_loop();
}
 
