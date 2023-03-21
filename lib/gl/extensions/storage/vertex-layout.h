#pragma once

#include <vector>
#include <cstddef>

namespace gl {

    // ----------------------------- VERTEX LAYOUT ELEMENT -----------------------------

    class vertex_layout;

    struct vertex final {
    private:
        unsigned int type_id;
        size_t count;
        size_t size;

    public:
        vertex(unsigned int type_id, size_t count, size_t size);

        template <typename vertex_type>
        static vertex_layout of_type(size_t count);       

        operator vertex_layout();

        friend class vertex_array;
    };

    // --------------------------------- VERTEX LAYOUT ---------------------------------

    class vertex_layout final {
    private:
        std::vector<vertex> vertices;

    public:
        vertex_layout();
        vertex_layout(std::initializer_list<vertex> vertices_initializer);

        vertex_layout& operator+(const vertex_layout& other);

        friend class vertex_array;
    };

    vertex_layout operator+(const vertex& first, const vertex& second);

    template <typename target_type>
    vertex_layout layout(size_t count = 1) {
        return vertex::of_type<target_type>(count);
    }

};
