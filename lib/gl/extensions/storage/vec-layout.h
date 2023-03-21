#pragma once

#include "vertex-layout.h"
#include <assert.h>

namespace math {

    template <typename element_type, size_t count>
    gl::vertex_layout vector_layout(const size_t number_of_vectors = 1) {
        assert(number_of_vectors > 0);

        gl::vertex_layout layout = gl::vertex::of_type<element_type>(count);
        for (size_t i = 0; i < number_of_vectors - 1; ++ i)
            layout = layout + gl::vertex::of_type<element_type>(count);

        return layout;
    }

}
