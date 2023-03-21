#include "renderer.h"

namespace gl {

    renderer::renderer(): is_set_up(false) {}

    void renderer::setup_ensure_once(GLFWwindow* current_window, const size_t new_width, const size_t new_height) {
        this->width  = new_width;  // Setup renderer's dimensions 
        this->height = new_height;

        if (!is_set_up) {
            this->glfw_window = current_window;

            // Setup, just this once
            setup(); is_set_up = true;
        }
    }
 
    GLFWwindow* renderer::get_glfw_window() const { return this->glfw_window; }

    size_t renderer::get_width()  const { return this->width;  }
    size_t renderer::get_height() const { return this->height; }

};
