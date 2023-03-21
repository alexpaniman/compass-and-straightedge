# Setup m4 preprocessor for opengl-wrapper.h.m4

add_custom_command(OUTPUT opengl-wrapper.h
    COMMAND m4 -I ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/opengl-wrapper.h.m4 >
      ${CMAKE_CURRENT_SOURCE_DIR}/opengl-wrapper.h
    DEPENDS opengl-wrapper.h.m4
    COMMENT "Generating wrapper for OpenGL calls" )

add_custom_target(run-m4-opengl-wrapper-generator
    DEPENDS opengl-wrapper.h
    COMMENT "Checking if opengl-wrapper.h re-generation is required" )

add_dependencies(gl run-m4-opengl-wrapper-generator)
