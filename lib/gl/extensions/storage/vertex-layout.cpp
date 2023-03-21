#include "vertex-layout.h"
#include "GL/glew.h"

namespace gl {

    // ------------------------------- VERTEX DEFINITION -------------------------------

    template <>
    vertex_layout vertex::of_type<float>(size_t count) {
        return {{ GL_FLOAT, count, count * sizeof(float) }};
    }

    template <>
    vertex_layout vertex::of_type<double>(size_t count) {
        return {{ GL_DOUBLE, count, count * sizeof(double) }};
    }

    template <>
    vertex_layout vertex::of_type<int>(size_t count) {
        return {{ GL_INT, count, count * sizeof(int) }};
    }

    template <>
    vertex_layout vertex::of_type<unsigned int>(size_t count) {
        return {{ GL_UNSIGNED_INT, count, count * sizeof(unsigned int) }};
    }

    vertex::operator vertex_layout() {
        return vertex_layout { *this };
    }

    // -------------------------- VERTEX LAYOUT COMPOSITON DSL --------------------------

    vertex_layout::vertex_layout(): vertices() {}

    vertex_layout::vertex_layout(std::initializer_list<vertex> vertices_initializer)
        : vertices(vertices_initializer) {}

    vertex_layout& vertex_layout::operator+(const vertex_layout& other) {
        for (vertex element: other.vertices)
            vertices.push_back(element);

        return *this;
    }

    // --------------------------------- VERTEX LAYOUT ---------------------------------

    vertex::vertex(const unsigned int type_id, const size_t count, const size_t size):
        type_id(type_id), count(count), size(size) {};

};
