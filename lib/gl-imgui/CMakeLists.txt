add_library(gl-imgui STATIC imgui-renderer.cpp ui-window.cpp)

target_include_directories(
    gl-imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(gl-imgui PUBLIC gl imgui)
