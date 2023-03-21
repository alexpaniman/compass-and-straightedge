#include "imgui-renderer.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "renderer.h"
#include <exception>

namespace gl::imgui {

    void imgui_renderer::setup_current_renderer(renderer* renderer_to_setup) {
        if (renderer_to_setup == nullptr)
            return;

        renderer_to_setup->
            setup_ensure_once(this->get_glfw_window(), get_width(), get_height());
    }

    void imgui_renderer::set_ui_renderer(renderer* new_renderer) {
        this->ui_renderer = new_renderer;
        setup_current_renderer(ui_renderer);
    }

    void imgui_renderer::set_main_renderer(renderer* new_renderer) {
        this->main_renderer = new_renderer;
        setup_current_renderer(main_renderer);
    }

    void imgui_renderer::set_main_renderer(std::function<void()>&& new_main_renderer) {
        set_main_renderer(std::make_unique<gl::function_renderer>(new_main_renderer));
    }

    void imgui_renderer::set_ui_renderer(std::function<void()>&& new_ui_renderer) {
        set_ui_renderer(std::make_unique<gl::function_renderer>(new_ui_renderer));
    }

    // OpenGL version to use with Dear ImGUI

    // This wrapper isn't designed to be portable,
    // and besides version 440 is pretty available
    // on PC's. So I'm just hardcoding it:
    static const std::string opengl_version = "#version 440"; 

    void imgui_renderer::setup() {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        // Dark theme is the best, we should make it default!
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(this->get_glfw_window(), true);
        ImGui_ImplOpenGL3_Init(opengl_version.c_str());

        // Setup renderers
        setup_current_renderer(ui_renderer);
        setup_current_renderer(main_renderer);
    };

    void imgui_renderer::draw()  {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        // Draw whatever else we might be rendering
        main_renderer->draw();
        ui_renderer  ->draw();

        // Draw UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    };

    imgui_renderer::~imgui_renderer() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }

}
