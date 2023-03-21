#include "gl-imgui.h"
#include "ui-window.h"

using namespace math;

class compass_and_straightedge: public gl::imgui::ui_window {
public:
    using gl::imgui::ui_window::ui_window;

    void draw_ui() override {
        ImGui::Begin("List of shapes");

        for (const auto& colored_shape: { "Hello", "World", "My", "Attack on Titan" })
            ImGui::Selectable(colored_shape);

        ImGui::End();
    }

    void draw_main(gl::drawing_manager mgr) override {
        mgr.set_color({ 1.0f, 0.2f, 0.5f });
        mgr.draw_rectangle({ -1.0f, -1.0f }, { 1.0f, 1.0f });
    }
};

int main() {
    compass_and_straightedge drawer(1080, 1080, "Compass and Straightedge | OpenGL");
    drawer.draw_loop();
}
 
