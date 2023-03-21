#include "renderer-handler-window.h"
#include "renderer.h"

namespace gl {

    void renderer_handler_window::setup_current_renderer() {
        if (current_renderer == nullptr)
            return;

        current_renderer->setup_ensure_once(this->get_glfw_window(),
                                            (size_t) this->width,
                                            (size_t) this->height);
    }

    void renderer_handler_window::setup() {
        window_setup();
        setup_current_renderer();
    }

    void renderer_handler_window::draw()  {
        if (current_renderer != nullptr)
            current_renderer->draw();

        window_draw();
    }

    void renderer_handler_window::set_renderer(renderer* new_renderer) {
        setup_current_renderer();
        this->current_renderer = new_renderer;
    }

};
