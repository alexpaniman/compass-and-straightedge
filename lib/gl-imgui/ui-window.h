#pragma once

#include "drawing-manager.h"
#include "opengl-setup.h"
#include "renderer-handler-window.h"
#include "imgui-renderer.h"
#include "simple-drawing-renderer.h"
#include "imgui.h"

#include <functional>

namespace gl::imgui {

    class ui_window;

    namespace details {

        class ui_vector_drawing_adapter {
        public:
            ui_vector_drawing_adapter(ui_window &window)
                : m_window(window) {}

            void operator() (drawing_manager &mgr);

        private:
            ui_window &m_window;
        };

    }

    class ui_window: public gl::renderer_handler_window {
    public:
        using gl::renderer_handler_window::renderer_handler_window;

        void window_draw() override final {}

        virtual void draw_ui() {}
        virtual void draw_main([[maybe_unused]] gl::drawing_manager mgr) {}

        virtual void init() {}

        void window_setup() override final;

        void on_key_event(gl::key pressed_key, gl::action action) override {
            // TODO: fix imgui events for this:

            // ImGuiIO& io = ImGui::GetIO();
            // io.AddKeyEvent(static_cast<ImGuiKey>(pressed_key),
            //                action == action::PRESS || action == action::REPEAT);

            // if (io.WantCaptureKeyboard)
            on_key_pressed(pressed_key, action);
        }

        virtual void on_key_pressed(gl::key key, gl::action action) {}


        void on_mouse_move_event(math::vec2 cursor) override {
            ImGuiIO& io = ImGui::GetIO();

            double xpos, ypos;
            glfwGetCursorPos(get_glfw_window(), &xpos, &ypos);

            io.AddMousePosEvent(xpos, ypos);

            if (!io.WantCaptureMouse)
                on_mouse_moved(cursor);
        }

        virtual void on_mouse_moved([[maybe_unused]] math::vec2 cursor) {}


        void on_mouse_button_event(math::vec2 cursor, gl::mouse_button button, gl::action action) override {
            ImGuiIO& io = ImGui::GetIO();
            io.AddMouseButtonEvent(static_cast<int>(button), action == gl::action::PRESS);

            if (!io.WantCaptureMouse)
                on_mouse_pressed(cursor, button, action);
        }

        virtual void on_mouse_pressed(math::vec2 cursor, gl::mouse_button button, gl::action action) {}

    private:
        simple_drawing_renderer<details::ui_vector_drawing_adapter> vector_renderer =
            { details::ui_vector_drawing_adapter(*this) };

        gl::imgui::imgui_renderer ui_renderer;
    };

    namespace details {

        inline void ui_vector_drawing_adapter::operator() (drawing_manager &mgr) {
            m_window.draw_main(mgr);
        }

    }

};
