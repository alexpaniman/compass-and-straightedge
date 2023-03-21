#pragma once

#include "opengl-setup.h"

namespace gl::uniform {

    int get_uniform_location(const shaders::shader_program& program,
                             const std::string uniform_name);

}
