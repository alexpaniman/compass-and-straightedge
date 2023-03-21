add_library(gl STATIC
    # Wrappers
    wrappers/proxy/opengl-error-handler.cpp

    wrappers/objects/vertex-array.cpp
    wrappers/objects/uniforms.cpp
    wrappers/objects/vertex-buffer.cpp

    wrappers/setup/opengl-setup.cpp

    # Extensions
    extensions/storage/vertex-layout.cpp

    extensions/renderer/renderer.cpp
    extensions/renderer/renderer-handler-window.cpp

    extensions/simple-drawer/drawing-manager.cpp)

target_include_directories(gl PUBLIC
    # Common interface
    ${CMAKE_CURRENT_SOURCE_DIR}

    # Wrappers
    ${CMAKE_CURRENT_SOURCE_DIR}/wrappers/objects/
    ${CMAKE_CURRENT_SOURCE_DIR}/wrappers/proxy/
    ${CMAKE_CURRENT_SOURCE_DIR}/wrappers/setup/

    # Extensions
    ${CMAKE_CURRENT_SOURCE_DIR}/extensions/renderer/
    ${CMAKE_CURRENT_SOURCE_DIR}/extensions/simple-drawer/
    ${CMAKE_CURRENT_SOURCE_DIR}/extensions/storage/

    # Math
    ${CMAKE_CURRENT_SOURCE_DIR}/math/
  )

# Generate OpenGL wrappers

add_subdirectory(wrappers/proxy)

# Link OpenGL, GLEW and GLFW with gl

set(OpenGL_GL_PREFERENCE GLVND)

find_package(GLEW   REQUIRED)
find_package(glfw3  REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(
    gl PUBLIC
    ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

target_link_libraries(gl PUBLIC ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw axp)
