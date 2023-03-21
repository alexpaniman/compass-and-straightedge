#include "ui-window.h"

namespace gl::imgui {

    void ui_window::window_setup() {
        ui_renderer.set_ui_renderer(  std::bind(&ui_window::draw_ui  , this));
        ui_renderer.set_main_renderer(&vector_renderer);

        set_renderer(&ui_renderer);

        init();
    }

} // namespace gl::imgui
