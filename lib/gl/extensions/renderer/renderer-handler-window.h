#pragma once

#include "opengl-setup.h"
#include "renderer.h"

namespace gl {

    class renderer_handler_window: public gl::window {
    private:
        renderer* current_renderer = nullptr;
        void setup_current_renderer();

    public:
        using gl::window::window;

        void setup() override;
        void draw()  override;

        // Additional option for window to add something
        virtual void window_setup() {}
        virtual void window_draw()  {}

        void set_renderer(renderer* new_renderer);

        virtual ~renderer_handler_window() = default;
    };

};
