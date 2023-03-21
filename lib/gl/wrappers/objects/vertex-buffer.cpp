#include "vertex-buffer.h"
#include "opengl-wrapper.h"

namespace gl {

    static unsigned int generate_buffer_id() {
        unsigned int id = 0;
        gl::raw::gen_buffers(1, &id);

        return id;
    }

    vertex_buffer::vertex_buffer()
        : id(generate_buffer_id()), data({ NULL, 0 }) {}

    vertex_buffer::vertex_buffer(raw_data new_data)
        : vertex_buffer() {

        set_data(new_data);
    }

    vertex_buffer::~vertex_buffer() {
        // gl::raw::delete_buffers(1, &id);
    }

    void vertex_buffer::set_data(raw_data new_data) {
        this->data = new_data;

        bind();
        gl::raw::buffer_data(GL_ARRAY_BUFFER, (int) data.size,
                             data.data, GL_DYNAMIC_DRAW);
    }

    size_t vertex_buffer::size() const {
        return data.size;
    }

    void vertex_buffer::bind() const {
        gl::raw::bind_buffer(GL_ARRAY_BUFFER, id);
    }

};
