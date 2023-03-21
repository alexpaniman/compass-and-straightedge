#pragma once

#include "renderer.h"
#include <memory>
#include <string>
#include <utility>

namespace gl::imgui {

    // Use this wrapper to render ImGUI UI
    // alongside whatever you're rendering
    class imgui_renderer: public renderer {
    private:
        std::vector<std::unique_ptr<renderer>> owned_renderers;
        
        renderer* main_renderer;
        renderer* ui_renderer;

        void setup_current_renderer(renderer* renderer_to_setup);

    public:
        void setup() override;
        void draw()  override;

        // Sets renderer, doesn't own them
        void set_main_renderer(renderer* new_main_renderer);
        void set_ui_renderer(renderer* new_ui_renderer);

        // Sets renderers and owns them
        void set_main_renderer(std::unique_ptr<renderer>&& new_main_renderer) {
            owned_renderers.push_back(std::move(new_main_renderer));
            set_main_renderer(&*owned_renderers.back());
        }

        void set_ui_renderer(std::unique_ptr<renderer>&& new_ui_renderer) {
            owned_renderers.push_back(std::move(new_ui_renderer));
            set_ui_renderer(&*owned_renderers.back());
        }

        // Sets renderers from functions and owns them
        void set_main_renderer(std::function<void()>&& new_main_renderer);
        void set_ui_renderer(std::function<void()>&& new_ui_renderer);

        virtual ~imgui_renderer();
    };

}
