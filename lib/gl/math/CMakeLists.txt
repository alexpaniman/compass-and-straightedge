add_library(my-math STATIC null.cpp)

target_include_directories(my-math PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(my-math PUBLIC axp)
