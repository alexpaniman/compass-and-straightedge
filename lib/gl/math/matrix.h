#pragma once

#include <bits/utility.h>
#include <concepts>
#include <cstddef>
#include <filesystem>
#include <initializer_list>
#include <ostream>

#include "vec.h"
#include "metaprogramming.h"

template <typename element_type, std::size_t row_count, std::size_t column_count>
class matrix {
public:
    matrix() {
        for (auto& elements: m_elements)
            :: new(&elements) element_type(); // Default construct all elements!
    }

    matrix(std::same_as<math::vec<element_type, column_count>> auto&&... row_vecs) {
        std::size_t i = 0;
        ([&] { // Row-major copy
            auto *begin = m_elements + (i ++) * column_count;
            for (auto&& element: row_vecs)
                *(begin ++) = element;
        } (), ...);
    }
    
    element_type& operator[](std::size_t row, std::size_t column) {
        return m_elements[row * column_count + column];
    }

    const element_type& operator[](std::size_t row, std::size_t column) const {
        return m_elements[row * column_count + column];
    }

    template <size_t other_column_count>
    auto operator*(const matrix<element_type, column_count, other_column_count> &other) const {
        matrix<element_type, row_count, other_column_count> multiplied;
        for (std::size_t i = 0; i < row_count; ++ i)
            for (std::size_t j = 0; j < other_column_count; ++ j)
                for (std::size_t k = 0; k < column_count; ++ k)
                    multiplied[i, j] += (*this)[i, k] * other[k, j];

        return multiplied;
    }

    auto operator*(const math::vec<element_type, column_count> &other) const { // TODO: rewrite so it works for any vector
        math::vec<element_type, row_count> multiplied = math::zero;
        for (std::size_t i = 0; i < row_count; ++ i)
            for (std::size_t k = 0; k < column_count; ++ k)
                multiplied[i] += (*this)[i, k] * other[k];

        return multiplied;
    }

    friend std::ostream& operator<<(std::ostream &os, const matrix<element_type, row_count, column_count>& matrix) {
        for (std::size_t i = 0; i < row_count; ++ i) {
            bool is_first = i == 0, is_last = i == row_count - 1;

            if      (is_first)
                os << "⎛";
            else if (is_last)
                os << "⎝";
            else 
                os << "⎟";

            for (std::size_t j = 0; j < column_count; ++ j) {
                os << matrix[i, j];

                if (j != column_count - 1)
                    os << " ";
            }

            if      (is_first)
                os << "⎞";
            else if (is_last)
                os << "⎠";
            else 
                os << "⎟";

            os << "\n";
        }

        return os;
    }

private:
    element_type m_elements[row_count * column_count];
};

template <typename... vec_types, typename defining_vec = axp::get_t<0, vec_types...>>
matrix(vec_types&&... row_vecs)
    -> matrix<typename defining_vec::value_type, sizeof...(row_vecs), defining_vec::size()>;
