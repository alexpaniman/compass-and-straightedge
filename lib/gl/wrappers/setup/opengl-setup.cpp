#include "opengl-setup.h"
#include "vec.h"
#include "uniforms.h"
#include "vertex-array.h"

#include "opengl-wrapper.h"
#include "vertex-vector-array.h"

#include <GLFW/glfw3.h>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cassert>
#include <chrono>
#include <thread>


namespace gl {

    // -------------------------------- SHADERS GENERAL --------------------------------

    std::map<const std::string, const shaders::shader_type> shaders::shader_names = {
        std::pair(  "vertex", shaders::shader_type::VERTEX),
        std::pair("fragment", shaders::shader_type::FRAGMENT)
    };

    shaders::compiled_shader shaders::compile_shader(const shaders::raw_shader& shader_to_compile) {
        const shaders::shader_type type = shader_names[shader_to_compile.type];

        unsigned int id = glCreateShader((unsigned int) type);

        const char* src = shader_to_compile.source_code.c_str();
        gl::raw::shader_source(id, 1, &src, nullptr);
        gl::raw::compile_shader(id);

        int result;
        gl::raw::get_shaderiv(id, GL_COMPILE_STATUS, &result);

        if (result == GL_FALSE) {
            int length = 0;
            gl::raw::get_shaderiv(id, GL_INFO_LOG_LENGTH, &length);

            assert(length > 0 && "Length should be > 0");

            char* message = new char[(unsigned int) length];
            gl::raw::get_shader_info_log(id, length, &length, message);

            std::string error_message = "Failed to compile shader!\n";
            error_message += "==>    type: " + shader_to_compile.type + "\n";
            error_message += "==> message: \n" + std::string(message);

            // Because /message/ was already copied to /error_message/
            delete[] message;

            throw std::runtime_error(error_message);
        }

        return shaders::compiled_shader { type, id };
    }

    std::vector<shaders::compiled_shader>
    shaders::compile_shaders(const std::vector<shaders::raw_shader> raw_shaders) {

        std::vector<shaders::compiled_shader> compiled_shaders;
        compiled_shaders.reserve(raw_shaders.size());

        for (auto raw_shader: raw_shaders)
            compiled_shaders.push_back(compile_shader(raw_shader));

        return compiled_shaders;
    }

    std::vector<shaders::raw_shader> shaders::extract_shaders(const std::string filename) {
        std::ifstream input_file(filename);

        std::vector<shaders::raw_shader> shaders;

        std::string line;
        while (std::getline(input_file, line)) {
            if (line.find("#shader") == std::string::npos) {
                if (shaders.empty())
                    continue;

                shaders.back().source_code.append(line + "\n");
            } else {
                std::istringstream words(line);

                // Skip our directive "#shader":
                words.ignore(std::numeric_limits<std::streamsize>::max(), ' ');

                std::string shader_type_name; 
                std::getline(words, shader_type_name, ' ');

                shaders.push_back({ shader_type_name, {} });
            }
        }

        return shaders;
    }

    // --------------------------------- SHADER PROGRAM --------------------------------

    shaders::shader_program::shader_program(): id(glCreateProgram()) {}

    shaders::shader_program::shader_program(const std::string filename): shader_program() {
        from_file(filename);
    }

    void shaders::shader_program::from_shaders(const std::vector<shaders::compiled_shader> raw_shaders) {
        for (auto shader: raw_shaders)
            gl::raw::attach_shader(id, shader.id);

        gl::raw::link_program(id);
        gl::raw::validate_program(id);

        // for (auto shader: raw_shaders)
        //     gl::raw::delete_program(shader.id);
    };

    void shaders::shader_program::from_shaders(const std::vector<shaders::raw_shader> raw_shaders) {
        std::vector<shaders::compiled_shader> compiled_shaders =
            shaders::compile_shaders(raw_shaders);

        from_shaders(compiled_shaders);
    }


    void shaders::shader_program::from_file(const std::string filename) {
        return from_shaders(shaders::extract_shaders(filename));
    }

    unsigned int shaders::shader_program::get_id() const {
        return id;
    }

    void shaders::shader_program::bind() const {
        gl::raw::use_program(id);
    }

    shaders::shader_program::~shader_program() {
        gl::raw::delete_program(id);
    }

    int shaders::shader_program::get_uniform_location_cached(std::string name) const {
        int location = 0;
        if (this->uniform_locations.contains(name))
            location = uniform_locations[name];
        else {
            location = gl::uniform::get_uniform_location(*this, name);
            if (location == -1)
                throw std::runtime_error("uniform is unused in shader: '" + name + "'");

            uniform_locations[name] = location;
        }

        return location;
    }

    #define DEFINE_UNIFORM_SETTER(type, prefix, setter)                                                      \
        template <>                                                                                          \
        void shaders::shader_program::uniform(std::string name, type value) const {                          \
            bind();                                                                                          \
            gl::raw::uniform##prefix(get_uniform_location_cached(name), setter);                             \
        }

    DEFINE_UNIFORM_SETTER(int   , 1i, value)
    DEFINE_UNIFORM_SETTER(double, 1d, value)
    DEFINE_UNIFORM_SETTER(float , 1f, value)

    #define _ ,
    #define DEFINE_VECTOR_UNIFORM_SETTERS(type, prefix)                                                      \
        DEFINE_UNIFORM_SETTER(math::vec<type _ 4>, 4##prefix, value.x() _ value.y() _ value.z() _ value.w()) \
        DEFINE_UNIFORM_SETTER(math::vec<type _ 3>, 3##prefix, value.x() _ value.y() _ value.z())             \
        DEFINE_UNIFORM_SETTER(math::vec<type _ 2>, 2##prefix, value.x() _ value.y())                         \

    DEFINE_VECTOR_UNIFORM_SETTERS(int   , i)
    DEFINE_VECTOR_UNIFORM_SETTERS(double, d)
    DEFINE_VECTOR_UNIFORM_SETTERS(float , f)

    #undef _
    #undef DEFINE_UNIFORM_SETTER
    #undef DEFINE_VECTOR_UNIFORM_SETTERS

    // ---------------------------------- GLFW WINDOW ----------------------------------

    // GLFWwindow <=> gl::window mapping for deducing gl::window in key callback
    static std::map<GLFWwindow*, gl::window*> window_mapping {};

    window::window(const int width, const int height, const char* title)
        : width(width), height(height), current_fps(0), initial_size { width, height } {

        if (!glfwInit())
            throw std::runtime_error("Failed to initialize glfw!");

        static constexpr int sample_count = 16;
        glfwWindowHint(GLFW_SAMPLES, sample_count);

        glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);

        if (glfw_window == NULL) {
            glfwTerminate();

            const char* error_message = NULL;
            glfwGetError(&error_message);

            throw std::runtime_error("Failed to open window!\n" +
                                     std::string(error_message));
        }

        window_mapping[glfw_window] = this;

        bind();
        glfwSwapInterval(1); // Let's try to enable VSync

        if (glewInit() != GLEW_OK)
            throw std::runtime_error("Failed to initialize glew!");

        glEnable(GL_MULTISAMPLE);
    }
    
    void window::bind() const {
        glfwMakeContextCurrent(glfw_window);
    }


    int window::get_fps() const noexcept {
        return current_fps;
    }

    GLFWwindow* window::get_glfw_window() const noexcept {
        return this->glfw_window;
    }

    static void key_press_callback(GLFWwindow* window, int key, int /* scancode */, int action, int /* mods */) {
        // Unused for now (maybe in the future this could take advantage of them):
        //     scancode & mods

        window_mapping[window]->on_key_event(static_cast<gl::key>(key), static_cast<gl::action>(action));
    }

    static void mouse_press_callback(GLFWwindow* window, double /* xpos */, double /* ypos */) {
        gl::window &current_window = *window_mapping[window];

        auto cursor = current_window.get_cursor_position();
        if (!cursor) // cursor is outside window
            return;

        current_window.on_mouse_move_event(*cursor);
    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int /* mods */) {
        gl::window &current_window = *window_mapping[window];

        auto cursor = current_window.get_cursor_position();

        if (cursor) {
            current_window.on_mouse_button_event(*cursor,
                static_cast<gl::mouse_button>(button),
                static_cast<gl::action>(action));
        }

        // TODO: Pool mouse position

    }

    std::optional<math::vec2> window::get_cursor_position() const {
        double xpos, ypos;
        glfwGetCursorPos(get_glfw_window(), &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(get_glfw_window(), &width, &height);

        if (xpos > this->width)
            return {};

        ypos -= height;
        ypos += this->height;

        if (ypos < 0)
            return {};

        math::axes ax = default_scaling();
        return ax.get_world_coordinates({
            static_cast<float>(xpos / (this->width  / 2.0) - 1.0),
            static_cast<float>(ypos / (this->height / 2.0) - 1.0)
        });
    }

    void window::redraw() {
        draw();
    }

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        gl::window &current_window = *window_mapping[window];

        math::vec<int, 2> old_size = { current_window.width, current_window.height };

        current_window.width = width;
        current_window.height = height;

        glViewport(0, 0, width, height);

        current_window.on_resize_event(old_size);
        current_window.redraw();
    }

    void window::draw_loop() {
        setup();

        glfwSetKeyCallback(this->glfw_window, &key_press_callback);
        glfwSetCursorPosCallback(this->glfw_window, &mouse_press_callback);
        glfwSetMouseButtonCallback(this->glfw_window, &mouse_button_callback);
        glfwSetFramebufferSizeCallback(this->glfw_window, &framebuffer_size_callback);

        static int fps_counter = 0;

        bool redraw_in_loop = should_redraw_in_loop();

        if (!redraw_in_loop) {
            redraw(); // allow lazy configuration to happen
            redraw(); // initial draw
        }

        double last_time = glfwGetTime();
        while (!glfwWindowShouldClose(glfw_window)) {
            double frame_start_time = glfwGetTime();

            if (redraw_in_loop)
                redraw();

            glfwSwapBuffers(glfw_window);
            glfwPollEvents();

            double current_time = glfwGetTime();
            fps_counter ++;
            if (current_time - last_time >= 1.0) {
                current_fps = fps_counter;
                on_fps_updated();

                fps_counter = 0;
                last_time = current_time;
            }

            using namespace std::chrono_literals;

            double target_fps = desired_fps();

            // sync with desired frame rate:
            if (target_fps != -1) {
                double remaining = 1. / target_fps - (current_time - frame_start_time);
                std::this_thread::sleep_for(remaining * 1s);
            }
        }
    }

    window::~window() {
        glfwTerminate();
    }

    // ------------------------------------ DRAWING ------------------------------------

    void draw(drawing_type type, const vertex_array& array) {
        array.bind();
        gl::raw::draw_arrays((unsigned int) type, 0, (int) array.get_element_count());
    }

    void draw(drawing_type type, const vertex_array& array, const shaders::shader_program& shaders) {
        shaders.bind();
        draw(type, array);
    }
}
