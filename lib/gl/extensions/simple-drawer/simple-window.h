#pragma once

#include "drawing-manager.h"
#include "renderer-handler-window.h"
#include "simple-drawing-renderer.h"
#include <memory>


namespace gl {

    class simple_drawing_window;

    namespace details {

        class simple_drawing_adapter {

        public:
            simple_drawing_adapter(simple_drawing_window &window)
                : m_window(window) {}

            void operator() (drawing_manager &mgr);

        private:
            simple_drawing_window &m_window;
        };

    }

    class simple_drawing_window: public gl::renderer_handler_window {
    public:
        simple_drawing_window(int width, int height, const char* name)
            : gl::renderer_handler_window(width, height, name) {

            set_renderer(&m_renderer);
        }

        virtual void loop_draw(drawing_manager mgr) = 0;

    private:
        simple_drawing_renderer<details::simple_drawing_adapter> m_renderer =
            { details::simple_drawing_adapter(*this) };

    };

    namespace details {

        inline void simple_drawing_adapter::operator() (drawing_manager &mgr) {
            m_window.loop_draw(mgr);
        }

    }

}
