#pragma once

#include <GLFW/glfw3.h>
#include <cstddef>
#include <functional>

namespace gl {

    class renderer {
    private:
        GLFWwindow* glfw_window;
        size_t width, height;

        bool is_set_up;

    public:
        renderer();

        virtual void setup() {};
        virtual void draw() = 0;

        size_t get_width()  const;
        size_t get_height() const;

        GLFWwindow* get_glfw_window() const;
        virtual void setup_ensure_once(GLFWwindow* current_window,
                                       size_t new_width,
                                       size_t new_height);

        virtual ~renderer() = default;
    };

    class function_renderer: public renderer {
    private:
        std::function<void()> draw_function;

    public:
        function_renderer(std::function<void()> new_function)
            : draw_function(new_function) {}

        void draw() override { draw_function(); }

        virtual ~function_renderer() = default;
    };

};
