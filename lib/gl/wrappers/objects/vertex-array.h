#pragma once

#include "vertex-buffer.h"
#include "vertex-layout.h"

#include <GL/glew.h>

#include <initializer_list>
#include <vector>

namespace gl {

    class vertex_array final {
    private:
        unsigned int id;
        size_t element_count;

        vertex_buffer buffer;
        vertex_layout layout;

    public:
        vertex_array();

        vertex_array(vertex_layout new_layout);
        vertex_array(vertex_layout new_layout, raw_data new_data);

        template <typename value_type>
        void assign(std::vector<value_type> data_buffer) {
            this->element_count = data_buffer.size();

            size_t layout_size = 0;
            for (vertex current: this->layout.vertices)
                layout_size += current.size;

            assign(this->layout, {
                    (void*) &(*data_buffer.begin()),
                    layout_size * data_buffer.size()
            });
        }

        template <typename value_type>
        void assign(vertex_layout new_layout, std::vector<value_type> data_buffer) {
            this->layout = new_layout;
            assign(data_buffer);
        }

        template <typename value_type>
        vertex_array(vertex_layout new_layout, std::vector<value_type> new_buffer)
            : vertex_array(new_layout) { assign(new_buffer); }

        void assign(raw_data new_buffer);
        void assign(vertex_layout new_layout, raw_data new_data);

        void set_layout(vertex_layout layout);

        size_t size() const;
        size_t get_element_count() const;

        void bind() const;

        ~vertex_array();
    };

};
