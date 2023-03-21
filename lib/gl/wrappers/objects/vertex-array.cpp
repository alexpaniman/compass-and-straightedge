#include "vertex-array.h"
#include "vertex-buffer.h"
#include "vertex-layout.h"

#include "opengl-wrapper.h"

namespace gl {
    vertex_array::vertex_array(): buffer(), layout() {
        gl::raw::gen_vertex_arrays(1, &this->id);
    }

    vertex_array::vertex_array(vertex_layout new_layout)
        : buffer(), layout(new_layout) {
        gl::raw::gen_vertex_arrays(1, &this->id);
    }

    void vertex_array::set_layout(vertex_layout new_layout) {
        this->layout = new_layout;
    }

    vertex_array::vertex_array(vertex_layout new_layout, raw_data new_data)
        : vertex_array(new_layout) { assign(new_data); }


    void vertex_array::assign(raw_data new_data) {
        this->buffer.set_data(new_data);
        
        this->bind();
        this->buffer.bind();

        unsigned int total_size = 0;
        for (vertex current: this->layout.vertices)
            total_size += current.size;

        unsigned int offset = 0;
        for (unsigned int i = 0; i < this->layout.vertices.size(); ++ i) {
            const vertex& layout_element = this->layout.vertices[i];

            gl::raw::enable_vertex_attrib_array(i);
            gl::raw::vertex_attrib_pointer(i, (int) layout_element.count,
                                           layout_element.type_id, GL_FALSE,
                                           (int) total_size,
                                           (const void*)(uintptr_t) offset);

            offset += layout_element.size;
        }
    }

    void vertex_array::assign(vertex_layout new_layout, raw_data new_data) {
        this->layout = new_layout;
        assign(new_data);
    }

    size_t vertex_array::get_element_count() const {
        return element_count;
    }

    size_t vertex_array::size() const {
        return buffer.size();
    }      

    vertex_array::~vertex_array() {}

    void vertex_array::bind() const { gl::raw::bind_vertex_array(this->id); }

};
