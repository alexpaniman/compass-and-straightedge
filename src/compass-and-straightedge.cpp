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

    void draw_ui() override {
        ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoDecoration);

        int* active_tool_ptr = reinterpret_cast<int*>(&active_tool_);
        ImGui::RadioButton(ICON_FA_TIMES_CIRCLE     " Point"  , active_tool_ptr, poke_point);
        ImGui::RadioButton(ICON_FA_DRAFTING_COMPASS " Compass", active_tool_ptr, compass);
        ImGui::RadioButton(ICON_FA_RULER            " Line"   , active_tool_ptr, ruler);

        ImGui::End();
    }

    // mgr.set_color({ 1.0f, 0.2f, 0.5f });
    void draw_main(gl::drawing_manager mgr) override {
        // draw background:
        mgr.set_color({ 0.13f, 0.11f, 0.12f });
        mgr.draw_rectangle({ -1.0f, -1.0f }, { 1.0f, 1.0f });

        // draw shapes:
        mgr.set_color({ 0.67f, 0.6f, 0.82f });
        mgr.set_width(0.007f);

        draw_shapes(mgr);

        if (auto cursor = get_cursor_position())
            draw_shape(mgr, *cursor);

        // draw their intersections:
        draw_points(mgr);
    }

    void on_mouse_moved(math::vec2 cursor) override {
        hovered_point_ = -1;

        for (std::size_t i = 0; i < points_.size(); ++ i)
            if ((points_[i] - cursor).len() <= 0.01f) {
                hovered_point_ = i; // select button when hovered over
                return;
            }
    }

    void on_mouse_pressed(math::vec2 cursor, gl::mouse_button button, gl::action action) override {
        if (button != gl::mouse_button::LEFT)
            return;

        if (action != gl::action::PRESS)
            return;

        if (active_tool_ == poke_point) {
            points_.emplace_back(cursor);
            manual_points_.emplace_back(cursor);
        } else if (selected_point_ != -1 && hovered_point_ != -1) {
            add_shape(points_[hovered_point_]);
            selected_point_ = -1;
            return;
        }
        
        selected_point_ = hovered_point_;
    }

private:
    // view of all available shapes:
    std::vector<std::unique_ptr<shape>> shapes_;
    std::vector<math::vec2> points_;
    std::vector<math::vec2> manual_points_;

    int selected_point_ = -1;
    int  hovered_point_ = -1;

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
        
        recalculate_intersections();
    }

    void draw_shape(gl::drawing_manager mgr, math::vec2 to) {
        if (active_tool_ == poke_point || selected_point_ == -1)
            return;

        math::vec2 from = points_[selected_point_];

        circle target_circle(from, to);
        line   target_line  (from, to);

        const shape *new_shape = nullptr;

        if      (active_tool_ == compass)
            new_shape = &target_circle;
        else if (active_tool_ ==   ruler)
            new_shape = &target_line;
        else {
            assert(false && "some shape is not handeled");
        }
        
        std::vector<math::vec2> intersection_points;
        for (auto &&shape: shapes_)
            shape->intersect(*new_shape, intersection_points);

        new_shape->draw(mgr);

        mgr.set_color({ 0.46f, 0.86f, 0.91f });
        for (auto &&point: intersection_points)
            draw_point(mgr, point);
    }

    void draw_point(gl::drawing_manager mgr, math::vec2 point) {
        constexpr double outer_point_size = 0.01;
        constexpr double inner_point_size = 0.007;

        circle(point, outer_point_size).fill(mgr);

        mgr.set_color({ 0.13f, 0.11f, 0.12f });
        circle(point, inner_point_size).fill(mgr); 
    }

    void recalculate_intersections() {
        points_.clear();
        for (std::size_t i = 0; i < shapes_.size(); ++ i)
            for (std::size_t j = i + 1; j < shapes_.size(); ++ j)
                shapes_[i]->intersect(*shapes_[j], points_);

        for (auto &&manual_point: manual_points_)
            points_.push_back(manual_point);

        for (int i = 0; i < (int) points_.size(); ++ i)
            for (int j = 0; j < (int) points_.size(); ++ j) {
                if (i == j)
                    break;

                if ((points_[i] - points_[j]).len() <= 1e-2) {
                    points_.erase(points_.begin() + (j --));
                    break;
                }
            }
        
    }

    void draw_points(gl::drawing_manager mgr) {
        for (int i = 0; i < static_cast<int>(points_.size()); ++ i) {
            if (selected_point_ == i)
                mgr.set_color({ 0.9f, 0.2f, 0.3f });
            else if (hovered_point_ == i)
                mgr.set_color({ 0.3f, 0.2f, 0.9f });
            else
                mgr.set_color({ .65f, .85f, .46f });

            draw_point(mgr, points_[i]);
        }
        
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
 
