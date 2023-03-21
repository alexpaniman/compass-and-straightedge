#pragma once

#include <cstddef>

namespace gl {

    struct raw_data final {
        void* data;
        size_t size;
    };

    class vertex_buffer final {
    private:
        unsigned int id; 
        raw_data data;

    public:
        vertex_buffer();
        vertex_buffer(raw_data new_data);

        vertex_buffer(const vertex_buffer&) = delete;
        vertex_buffer& operator=(const vertex_buffer&) = delete;

        ~vertex_buffer();

        void set_data(raw_data new_data);
        void bind() const;

        size_t size() const;
    };

};
