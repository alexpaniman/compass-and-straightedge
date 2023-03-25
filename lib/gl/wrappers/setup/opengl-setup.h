#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <initializer_list>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

#include "axes.h"
#include "math.h"
#include "rect.h"
#include "vec.h"
#include "vertex-array.h"
#include "vertex-vector-array.h"

namespace gl::shaders {
    enum class shader_type {
        VERTEX = GL_VERTEX_SHADER, FRAGMENT = GL_FRAGMENT_SHADER
    };

    extern std::map<const std::string, const shader_type> shader_names;


    struct raw_shader final {
        std::string type;
        std::string source_code;
    };

    struct compiled_shader final {
        const shader_type type;
        const unsigned int id;
    };

    std::vector<raw_shader> extract_shaders(std::string filename);

    compiled_shader compile_shader(const raw_shader& shader_to_compile);

    std::vector<compiled_shader>
        compile_shaders(std::vector<raw_shader> raw_shaders);


    class shader_program final {
    private:
        const unsigned int id;

    public:
        shader_program();
        shader_program(std::string filename);

        void bind() const;
        unsigned int get_id() const;

        void from_shaders(std::vector<compiled_shader> shaders);
        void from_shaders(std::vector<raw_shader>      shaders);

        void from_file(std::string filename);

        ~shader_program();

    private:
        mutable std::map<std::string, int> uniform_locations;
        int get_uniform_location_cached(std::string uniform_name) const;

    public:
        template <typename uniform_type>
        void uniform(std::string name, uniform_type value) const;
    };
}

namespace gl {

    enum class key {
        SPACE         = GLFW_KEY_SPACE,
        APOSTROPHE    = GLFW_KEY_APOSTROPHE,
        COMMA         = GLFW_KEY_COMMA,
        MINUS         = GLFW_KEY_MINUS,
        PERIOD        = GLFW_KEY_PERIOD,
        SLASH         = GLFW_KEY_SLASH,
        NUMBER_0      = GLFW_KEY_0,
        NUMBER_1      = GLFW_KEY_1,
        NUMBER_2      = GLFW_KEY_2,
        NUMBER_3      = GLFW_KEY_3,
        NUMBER_4      = GLFW_KEY_4,
        NUMBER_5      = GLFW_KEY_5,
        NUMBER_6      = GLFW_KEY_6,
        NUMBER_7      = GLFW_KEY_7,
        NUMBER_8      = GLFW_KEY_8,
        NUMBER_9      = GLFW_KEY_9,
        SEMICOLON     = GLFW_KEY_SEMICOLON,
        EQUAL         = GLFW_KEY_EQUAL,
        A             = GLFW_KEY_A,
        B             = GLFW_KEY_B,
        C             = GLFW_KEY_C,
        D             = GLFW_KEY_D,
        E             = GLFW_KEY_E,
        F             = GLFW_KEY_F,
        G             = GLFW_KEY_G,
        H             = GLFW_KEY_H,
        I             = GLFW_KEY_I,
        J             = GLFW_KEY_J,
        K             = GLFW_KEY_K,
        L             = GLFW_KEY_L,
        M             = GLFW_KEY_M,
        N             = GLFW_KEY_N,
        O             = GLFW_KEY_O,
        P             = GLFW_KEY_P,
        Q             = GLFW_KEY_Q,
        R             = GLFW_KEY_R,
        S             = GLFW_KEY_S,
        T             = GLFW_KEY_T,
        U             = GLFW_KEY_U,
        V             = GLFW_KEY_V,
        W             = GLFW_KEY_W,
        X             = GLFW_KEY_X,
        Y             = GLFW_KEY_Y,
        Z             = GLFW_KEY_Z,
        LEFT_BRACKET  = GLFW_KEY_LEFT_BRACKET,
        BACKSLASH     = GLFW_KEY_BACKSLASH,
        RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
        GRAVE_ACCENT  = GLFW_KEY_GRAVE_ACCENT,
        WORLD_1       = GLFW_KEY_WORLD_1,
        WORLD_2       = GLFW_KEY_WORLD_2,
        ESCAPE        = GLFW_KEY_ESCAPE,
        ENTER         = GLFW_KEY_ENTER,
        TAB           = GLFW_KEY_TAB,
        BACKSPACE     = GLFW_KEY_BACKSPACE,
        INSERT        = GLFW_KEY_INSERT,
        DELETE        = GLFW_KEY_DELETE,
        RIGHT         = GLFW_KEY_RIGHT,
        LEFT          = GLFW_KEY_LEFT,
        DOWN          = GLFW_KEY_DOWN,
        UP            = GLFW_KEY_UP,
        PAGE_UP       = GLFW_KEY_PAGE_UP,
        PAGE_DOWN     = GLFW_KEY_PAGE_DOWN,
        HOME          = GLFW_KEY_HOME,
        END           = GLFW_KEY_END,
        CAPS_LOCK     = GLFW_KEY_CAPS_LOCK,
        SCROLL_LOCK   = GLFW_KEY_SCROLL_LOCK,
        NUM_LOCK      = GLFW_KEY_NUM_LOCK,
        PRINT_SCREEN  = GLFW_KEY_PRINT_SCREEN,
        PAUSE         = GLFW_KEY_PAUSE,
        F1            = GLFW_KEY_F1,
        F2            = GLFW_KEY_F2,
        F3            = GLFW_KEY_F3,
        F4            = GLFW_KEY_F4,
        F5            = GLFW_KEY_F5,
        F6            = GLFW_KEY_F6,
        F7            = GLFW_KEY_F7,
        F8            = GLFW_KEY_F8,
        F9            = GLFW_KEY_F9,
        F10           = GLFW_KEY_F10,
        F11           = GLFW_KEY_F11,
        F12           = GLFW_KEY_F12,
        F13           = GLFW_KEY_F13,
        F14           = GLFW_KEY_F14,
        F15           = GLFW_KEY_F15,
        F16           = GLFW_KEY_F16,
        F17           = GLFW_KEY_F17,
        F18           = GLFW_KEY_F18,
        F19           = GLFW_KEY_F19,
        F20           = GLFW_KEY_F20,
        F21           = GLFW_KEY_F21,
        F22           = GLFW_KEY_F22,
        F23           = GLFW_KEY_F23,
        F24           = GLFW_KEY_F24,
        F25           = GLFW_KEY_F25,
        KP_0          = GLFW_KEY_KP_0,
        KP_1          = GLFW_KEY_KP_1,
        KP_2          = GLFW_KEY_KP_2,
        KP_3          = GLFW_KEY_KP_3,
        KP_4          = GLFW_KEY_KP_4,
        KP_5          = GLFW_KEY_KP_5,
        KP_6          = GLFW_KEY_KP_6,
        KP_7          = GLFW_KEY_KP_7,
        KP_8          = GLFW_KEY_KP_8,
        KP_9          = GLFW_KEY_KP_9,
        KP_DECIMAL    = GLFW_KEY_KP_DECIMAL,
        KP_DIVIDE     = GLFW_KEY_KP_DIVIDE,
        KP_MULTIPLY   = GLFW_KEY_KP_MULTIPLY,
        KP_SUBTRACT   = GLFW_KEY_KP_SUBTRACT,
        KP_ADD        = GLFW_KEY_KP_ADD,
        KP_ENTER      = GLFW_KEY_KP_ENTER,
        KP_EQUAL      = GLFW_KEY_KP_EQUAL,
        LEFT_SHIFT    = GLFW_KEY_LEFT_SHIFT,
        LEFT_CONTROL  = GLFW_KEY_LEFT_CONTROL,
        LEFT_ALT      = GLFW_KEY_LEFT_ALT,
        LEFT_SUPER    = GLFW_KEY_LEFT_SUPER,
        RIGHT_SHIFT   = GLFW_KEY_RIGHT_SHIFT,
        RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
        RIGHT_ALT     = GLFW_KEY_RIGHT_ALT,
        RIGHT_SUPER   = GLFW_KEY_RIGHT_SUPER,
        MENU          = GLFW_KEY_MENU,
    };

    enum class mouse_button {
        LEFT = GLFW_MOUSE_BUTTON_LEFT,
        MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
        RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
    };

    enum class action {
        PRESS = GLFW_PRESS,
        RELEASE = GLFW_RELEASE,
        REPEAT = GLFW_REPEAT,
    };

    class window {
    public:
        const math::vec<int, 2> initial_size;
        int width, height;

        window(int width, int height, const char* title);

        virtual int desired_fps() { return -1 /* uncapped */; };
        virtual bool should_redraw_in_loop() { return true; };

        // This class shouldn't be copied or moved
        window(const window&) = delete;
        window& operator=(const window&) = delete;

        void bind() const;

        int get_fps() const noexcept;
        GLFWwindow* get_glfw_window() const noexcept;

        void draw_loop();

        virtual void setup() {};
        virtual void draw() = 0;
        void redraw();

        virtual void on_fps_updated() {};

        virtual void on_resize_event([[maybe_unused]] math::vec<int, 2> old_size) {}
        virtual void on_key_event([[maybe_unused]] gl::key pressed_key, [[maybe_unused]] gl::action action) {}
        virtual void on_mouse_move_event([[maybe_unused]] math::vec2 cursor) {}
        virtual void on_mouse_button_event([[maybe_unused]] math::vec2 cursor,
                                           [[maybe_unused]] gl::mouse_button button,
                                           [[maybe_unused]] gl::action action){}

        virtual ~window();

        std::optional<math::vec2> get_cursor_position() const;

    protected:
        math::axes default_scaling() const {
            math::axes ax {};
            math::rectangle &view = ax.m_view;

            float end_x = 2. * initial_size.x() / (float) width  - 1.;
            float end_y = 2. * initial_size.y() / (float) height - 1.;

            view.x1 = { end_x, end_y };

            return ax;
        }

        math::rectangle screen() { return math::rectangle { screen_x0(), screen_x1() }; }

    private:
        int current_fps;
        GLFWwindow* glfw_window;

        math::vec2 screen_x1() {
            return { 2.f * width  / initial_size.x() - 1.f,
                     2.f * height / initial_size.y() - 1.f };
        }
        
        math::vec2 screen_x0() { return -1 * screen_x1(); }
    };

    enum class drawing_type {
        POINTS                   = GL_POINTS                  ,
        LINE_STRIP               = GL_LINE_STRIP              ,
        LINE_LOOP                = GL_LINE_LOOP               ,
        LINES                    = GL_LINES                   ,
        LINE_STRIP_ADJACENCY     = GL_LINE_STRIP_ADJACENCY    ,
        LINES_ADJACENCY          = GL_LINES_ADJACENCY         ,
        TRIANGLE_STRIP           = GL_TRIANGLE_STRIP          ,
        TRIANGLE_FAN             = GL_TRIANGLE_FAN            ,
        TRIANGLES                = GL_TRIANGLES               ,
        TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
        TRIANGLES_ADJACENCY      = GL_TRIANGLES_ADJACENCY     ,
        PATCHES                  = GL_PATCHES
    };

    void draw(drawing_type type, const vertex_array& array,
              const shaders::shader_program& shaders);

    void draw(drawing_type type, const vertex_array& array);

    template <typename value_type>
    void draw(drawing_type type, const vertex_vector_array<value_type>& array,
              const shaders::shader_program& shaders) {

        draw(type, array.get_vertex_array(), shaders);
    }

    template <typename value_type>
    void draw(drawing_type type, const vertex_vector_array<value_type>& array) {
        draw(type, array.get_vertex_array());
    }
}
