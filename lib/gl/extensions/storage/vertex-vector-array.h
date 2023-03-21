#pragma once

#include "vertex-array.h"
#include "vertex-buffer.h"
#include "vertex-layout.h"

#include <vector>

namespace gl {

    template <typename value_type>
    class vertex_vector_array: public std::vector<value_type> {
    public:
        vertex_vector_array(): m_element_array_holder() {}

        void set_layout(gl::vertex_layout layout) {
            m_element_array_holder.set_layout(layout);
        }

        const vertex_array& get_vertex_array() const {
            return m_element_array_holder;
        }

        void update() { m_element_array_holder.assign(*this); }

        void assign_and_update(std::initializer_list<value_type> init) {
            std::vector<value_type>::assign(init);
            update();
        }

    private:
        vertex_array m_element_array_holder;
    };

};
