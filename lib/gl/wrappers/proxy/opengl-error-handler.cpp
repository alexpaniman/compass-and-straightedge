#include "opengl-error-handler.h"

#include "GL/glew.h"
#include <sstream>
#include <stdexcept>

namespace gl::error {

    const std::map<const error_code, const std::string> error_descriptions = {
        { error_code::no_error, "No error has been recorded." },

        { error_code::invalid_enum, "An unacceptable value is specified "
          "for an enumerated argument. The offending command is ignored "
          "and has no other side effect than to set the error flag." },

        { error_code::invalid_value, "A numeric argument is out of range. "
          "The offending command is ignored and has no other side effect "
          "than to set the error flag." },

        { error_code::invalid_operation, "The specified operation is not "
          "allowed in the current state. The offending command is ignored "
          "and has no other side effect than to set the error flag." },

        { error_code::invalid_framebuffer_operation, "The framebuffer "
          "object is not complete. The offending command is ignored and "
          "has no other side effect than to set the error flag. " },

        { error_code::out_of_memory, "There is not enough memory left to "
          "execute the command. The state of the GL is undefined, except for "
          "the state of the error flags, after this error is recorded. " },

        { error_code::stack_underflow, "An attempt has been made to perform "
          "an operation that would cause an internal stack to underflow. " },

        { error_code::stack_overflow, "An attempt has been made to perform "
          "an operation that would cause an internal stack to overflow. " },
    };

    std::string describe_error(error_code code) {
        auto &[_, description] = *error_descriptions.find(code);
        return description;
    }

    void clear_error() {
        while (glGetError() != GL_NO_ERROR);
    }

    void check_error() {
        std::stringstream error_message;
        bool no_errors_detected = true;

        unsigned int error = GL_NO_ERROR;
        while ((error = glGetError()) != GL_NO_ERROR) {
            error_message << "==> opengl error [" << error << "]\n";
            error_message << "  | "
                          << describe_error((error_code) error)
                          << "\n\n";

            no_errors_detected = false;
        }

        if (!no_errors_detected)
            throw std::runtime_error(error_message.str());
    }

};
