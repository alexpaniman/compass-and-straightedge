#include "uniforms.h"
#include "opengl-setup.h"
#include "opengl-wrapper.h"

namespace gl::uniform {

    int get_uniform_location(const shaders::shader_program& program,
                             const std::string uniform_name) {
        return gl::raw::get_uniform_location(program.get_id(), uniform_name.c_str());
    }

}
