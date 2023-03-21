add_executable(compass-and-straightedge compass-and-straightedge.cpp)

target_include_directories(compass-and-straightedge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(compass-and-straightedge PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

target_link_libraries(compass-and-straightedge gl-imgui)
