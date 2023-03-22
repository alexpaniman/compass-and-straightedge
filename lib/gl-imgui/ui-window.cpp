#include "ui-window.h"

namespace gl::imgui {

    void ui_window::window_setup() {
        // ==> setup render window:

        ui_renderer.set_ui_renderer(  std::bind(&ui_window::draw_ui  , this));
        ui_renderer.set_main_renderer(&vector_renderer);

        set_renderer(&ui_renderer);

        // ==> call implementer's initialization code:

        init();
    }

} // namespace gl::imgui
