#pragma once

#include <cmath>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
#include <ostream>
#include <type_traits>
#include <tuple>
#include <utility>

namespace details {

    // If vector stores floating point values, use the same value for len type,
    // on the other hand, if it uses anything else fallback to double.
    template <typename element_type>
    using default_len_type =
        std::conditional_t<std::is_floating_point_v<element_type>, element_type, double>;

    template <typename vec_type, typename element_type>
    concept has_coordinates = requires(vec_type vec, int i) {
        { vec[i] } -> std::convertible_to<element_type>;
    };

    template <typename vec_type, typename element_type>
    concept right_multipliable = requires(vec_type vec, element_type value) {
        { vec * value };
    };

    template <typename value_type, typename callback_type>
    class catch_modifications_proxy {
    public:
        catch_modifications_proxy(value_type& editable_value, callback_type&& callback)
            : m_editable_value(editable_value), m_callback(callback) {}

        #define FORWARD_OPERATOR(name)                                         \
            template <typename new_value_type>                                 \
            catch_modifications_proxy& operator name(new_value_type&& value) { \
                m_callback(); /* Notify about assignment */                    \
                m_editable_value name std::forward<new_value_type>(value);     \
                return *this;                                                  \
            }

        // Forward all known assignment operators to source type (notify about whichever)
        FORWARD_OPERATOR(=)
        FORWARD_OPERATOR(+=)
        FORWARD_OPERATOR(-=)
        FORWARD_OPERATOR(/=)
        FORWARD_OPERATOR(*=)
        FORWARD_OPERATOR(^=)
        FORWARD_OPERATOR(%=)
        FORWARD_OPERATOR(&=)
        FORWARD_OPERATOR(|=)
        FORWARD_OPERATOR(>>=)
        FORWARD_OPERATOR(<<=)

        #undef FORWARD_OPERATOR

        // Get back original value, but read-only
        operator const value_type&() { return m_editable_value; }

    private:
        value_type& m_editable_value;
        callback_type m_callback;
    };

    template <typename element_type>
    struct strip_proxy { using type = element_type; };

    template <typename element_type, typename callback_type>
    struct strip_proxy<catch_modifications_proxy<element_type, callback_type>> {
        using type = element_type;
    };

    template <typename element_type>
    using strip_proxy_t = typename strip_proxy<element_type>::type;



    template <typename indexable_type>
    class random_access_iterator_proxy {
    public:
        constexpr random_access_iterator_proxy(indexable_type& indexable,
                                               size_t starting_index)
            : m_indexable(indexable), m_current_index(starting_index) {}

        constexpr random_access_iterator_proxy<indexable_type>& operator++() const {
            ++ m_current_index;
            return *this;
        }

        constexpr       auto  operator*()       { return m_indexable[m_current_index]; }
        constexpr const auto& operator*() const { return m_indexable[m_current_index]; }

        constexpr bool operator!=(const random_access_iterator_proxy<indexable_type>& other) const {
            return m_current_index != other.m_current_index;
        }

    private:
        indexable_type& m_indexable;

        mutable size_t m_current_index;
    };


    template <typename impl_type, typename element_type, size_t count>
    class vec_base_kernel {
    public:
        template<typename... vector_coordinates>
        constexpr vec_base_kernel(vector_coordinates... initializer_coordinates)
            : m_coordinates { initializer_coordinates... } {

            static_assert(sizeof...(vector_coordinates) == count,
                          "Invalid number of vector coordinates!");
        }

        constexpr auto operator[](const size_t index) {
            return catch_modifications_proxy(m_coordinates[index],
                    static_cast<impl_type*>(this)->get_change_callback());
        }

        constexpr const auto& operator[](const size_t index) const {
            return m_coordinates[index];
        }

    private:
        element_type  m_coordinates[count];
    };

    template <typename impl_type, typename element_type, size_t count>
    class vec_refs_kernel {
    public:
        template<typename... index_types>
        constexpr vec_refs_kernel(impl_type& original_vec, index_types... indices)
            : m_original_vec(original_vec), m_index_translation_map { indices... } {

            static_assert(sizeof...(indices) == count,
                          "Invalid number of vector coordinates!");
        }

        constexpr decltype(auto) operator[](const size_t index) {
            return m_original_vec[m_index_translation_map[index]];
        }

        constexpr decltype(auto) operator[](const size_t index) const {
            return m_original_vec[m_index_translation_map[index]];
        }

    private:
        size_t m_index_translation_map[count];
        impl_type& m_original_vec;
    };

    template<typename impl_type, typename... index_types>                                    
    vec_refs_kernel(impl_type& original_vec, index_types... indices) ->                    
        vec_refs_kernel<impl_type, decltype(original_vec[0]), sizeof...(indices)>;


    //        for CRTP            for switching kernels
    template <typename impl_type, typename vec_kernel_impl_type, 
              template <typename slice_vec_type,
                        typename slice_element_type,
                        size_t slice_element_count,
                        typename slice_len_type>
                typename vec_slice_impl_type,  // for instantiation of sliced vec
              typename element_type, size_t count, typename len_type>
    class vec_shell_base: public vec_kernel_impl_type {
    public:
        using vec_kernel_impl_type::vec_kernel_impl_type;

        // => The only way to interact with vector's components
        using vec_kernel_impl_type::operator[];

        // => Implement convenient coordinate getters for our vectors:

        // Make sure there is no vec.z() in two-dimensional vec (which has only x and y)
        #define STATIC_ASSERT_AVAILABILITY(coordinate_name, index)                 \
            static_assert(count > index, "Vector doesn't have `" #coordinate_name  \
                                         "' because it's too small!");

        #define COORDINATE_GETTER(coordinate_name, index)                          \
            constexpr auto coordinate_name() {                                     \
                STATIC_ASSERT_AVAILABILITY(coordinate_name, index)                 \
                return vec_kernel_impl_type::operator[](index);                    \
            }                                                                      \
                                                                                   \
            constexpr const element_type &coordinate_name() const {                \
                /* Relies on const ref conversion from returned by kernel [...] */ \
                return (*this)[index];                                             \
            }                                                                      \

        // There are conventional names for vector coordinates up to 4
        #define COORDINATE_GETTERS(name0, name1, name2, name3)                     \
            COORDINATE_GETTER(name0, 0) COORDINATE_GETTER(name1, 1)                \
            COORDINATE_GETTER(name2, 2) COORDINATE_GETTER(name3, 3)                \

        // ==> Define same coordinate getters that OpenGL uses:

        // Useful when accessing vectors that represent points or normals:
        COORDINATE_GETTERS(x, y, z, w)

        // Useful when accessing vectors that represent colors:
        COORDINATE_GETTERS(r, g, b, a)

        // Useful when accessing vectors that represent texture coordinates:
        COORDINATE_GETTERS(s, t, p, q)

        #undef COORDINATE_GETTERS
        #undef COORDINATE_GETTER

        #undef STATIC_ASSERT_AVAILABLE


        #define DEFINE_ASSIGNMENT(assignment)                                      \
            template <has_coordinates<element_type> other_vec>                     \
            constexpr impl_type& operator assignment(const other_vec& other) {     \
                for (size_t i = 0; i < count; ++ i)                                \
                    (*this)[i] assignment other[i];                                \
                                                                                   \
                return *get_impl(); /* CRTP Polymorphic chaining */                \
            }

        DEFINE_ASSIGNMENT(*=) DEFINE_ASSIGNMENT(/=)
        DEFINE_ASSIGNMENT(-=) DEFINE_ASSIGNMENT(+=)

        #undef DEFINE_ASSIGNMENT


        #define DEFINE_OPERATOR(name, corresponding_assignment)                    \
            template <has_coordinates<element_type> other_vec>                     \
            constexpr impl_type operator name(const other_vec& other) const {      \
                impl_type new_vec = *get_impl();                                   \
                return new_vec corresponding_assignment other;                     \
            }                                                                      \

        DEFINE_OPERATOR(*, *=) DEFINE_OPERATOR(-, -=)
        DEFINE_OPERATOR(+, +=) DEFINE_OPERATOR(/, /=)

        #undef DEFINE_OPERATOR

        constexpr random_access_iterator_proxy<impl_type> begin() {
            return random_access_iterator_proxy(*get_impl(),         0);
        }

        constexpr random_access_iterator_proxy<impl_type>   end() {
            return random_access_iterator_proxy(*get_impl(), count - 1);
        }

        constexpr const random_access_iterator_proxy<impl_type> begin() const {
            return random_access_iterator_proxy(*get_impl(),         0);
        }

        constexpr const random_access_iterator_proxy<impl_type>   end() const {
            return random_access_iterator_proxy(*get_impl(), count - 1);
        }

        constexpr impl_type& operator*=(const element_type value) {
            for (int i = 0; i < count; ++ i)
                (*this)[i] *= value;

            return *get_impl();
        }

        constexpr impl_type operator*(const element_type value) const {
            impl_type new_vec = *get_impl();
            return new_vec *= value;
        }

        template <right_multipliable<element_type> other_vec>
        friend constexpr impl_type operator*(const element_type value,
                                             const other_vec& other) {
            return other * value;
        }

        template <typename other_vector>
        constexpr element_type dot(const other_vector& other) const {
            element_type accumulator {};
            for (size_t i = 0; i < count; ++ i)
                accumulator += (*this)[i] * other[i];

            return accumulator;
        }

        constexpr len_type len() const {
            return static_cast<len_type>(sqrt(this->dot(*this)));
        }

        constexpr impl_type& normalize() {
            return (*this) *= (static_cast<len_type>(1) / len());
        }

        constexpr impl_type& rotate(double angle) {
            static_assert(count == 2, "Rotation for now is impemented only for 2D case!");

            x() = static_cast<element_type>(cos(angle) * x() - sin(angle) * y());
            y() = static_cast<element_type>(sin(angle) * x() + cos(angle) * y());

            return *get_impl();
        }

        // If you need to make current vector perpendicular to itself, you can use rotate!
        constexpr impl_type perpendicular() const {
            static_assert(count == 2, "Perpendicular for now is impemented only for 2D case!");

            return impl_type { y(), - x() };
        }

        constexpr impl_type rotated(double angle) const {
            impl_type new_vec = *get_impl();
            return new_vec.rotate(angle);
        }

        constexpr impl_type normalized() const {
            impl_type new_vec = *get_impl();
            return new_vec.normalize();
        }


        friend std::ostream& operator<<(std::ostream& os, const impl_type& vector) {
            os << "(";

            for (size_t i = 0; i < count; ++ i) {
                os << vector[i];
                if (i != count - 1) os << ", ";
            }

            return os << ")";
        }

        template <size_t... indicies>
        auto slice() {
            return vec_slice_impl_type { *get_impl(), indicies... };
        }

        // TODO: move inside?
        auto get_change_callback() {    // Referenced by kernel!
            return [this]() { get_impl()->notify_vector_changed(); };
        }

    private:
        // element_type m_coordinates[count];
        //              ^~~~~~~~~~~~~ coordinates are inherited from kernel

        // Simplify CRTP usage of implementation class
              impl_type* get_impl()       { return static_cast<      impl_type*>(this); }
        const impl_type* get_impl() const { return static_cast<const impl_type*>(this); }

        void notify_vector_changed() {} // Can be overloaded via CRTP

        auto& get_non_const_this_ref() const {
            return const_cast<std::remove_const_t<decltype(*this)>>(*this);
        }

    };

}

namespace math {

    // There's no way to use type alias in inheritance, or class specifier
    // (e.g friend), macro is cringe, but still will do the job.
    #define GENERIC_VEC_BASE_TYPE(impl, kernel)                                  \
        details::vec_shell_base<impl, kernel<impl, element_type, count>,         \
                                subvec, element_type, count, len_type>

    #define _ ,
    #define SUBVEC_BASE_TYPE                                                     \
        details::vec_shell_base<                                                 \
            subvec<vec_impl_type, element_type, count, len_type>,                \
            details::vec_refs_kernel<vec_impl_type, element_type, count>,        \
            subvec, element_type, count, len_type>

    template <typename vec_impl_type, typename element_type, size_t count, typename len_type>
    class subvec: public SUBVEC_BASE_TYPE {
    public:
        using SUBVEC_BASE_TYPE::vec_shell_base;
    };


    template<template <typename element_type,
                       size_t count, typename len_type>
             typename impl_type,
             typename element_type, size_t count, typename len_type, 
             typename... index_types>                                    
    subvec(impl_type<element_type, count, len_type>& original_vec, index_types... indices) ->                    
        subvec<impl_type<element_type, count, len_type>,
               element_type, sizeof...(indices), len_type>;

    // Deduction guide has to be defined twice (for both vecs), but they
    // have the exact same interface, so we'll use macro (there is no other way)

    #define VEC_CTAD(vec)                                                        \
        template<class... vector_coordinates>                                    \
        vec(vector_coordinates... initializer_coordinates) ->                    \
            vec<details::strip_proxy_t<                                          \
                    std::tuple_element_t<0, std::tuple<vector_coordinates...>>>, \
                sizeof...(vector_coordinates)>; // Deduce vec type by first element!

    #define VEC_BASE_TYPE                                                        \
        GENERIC_VEC_BASE_TYPE(vec<element_type _ count _ len_type>,              \
                              details::vec_base_kernel)

    inline // Makes uncached::vec "leak" in outer namespace, making it "default" in a way
    namespace uncached {

        template <typename element_type, size_t count,
                  typename len_type = details::default_len_type<element_type>>
        class vec: public VEC_BASE_TYPE {
        public:
            using VEC_BASE_TYPE::vec_shell_base;
        };

        VEC_CTAD(vec)

    }

    namespace cached {

        template <typename element_type, size_t count,
                  typename len_type = details::default_len_type<element_type>>
        class vec: public VEC_BASE_TYPE {
        public:
            using VEC_BASE_TYPE::vec_shell_base;

            constexpr len_type len() const /* CRTP override */ {
                if (!std::isnan(m_cached_length))
                    return m_cached_length;

                return m_cached_length = VEC_BASE_TYPE::len();
            }

        private:
            mutable len_type m_cached_length = std::numeric_limits<len_type>::quiet_NaN();

            // Do not expose notify_vector_changed, yet override it!
            friend class VEC_BASE_TYPE;

            void notify_vector_changed() /* CRTP override */ {
                // Invalidate current length, it will be recalculated on demand!
                m_cached_length = std::numeric_limits<len_type>::quiet_NaN();
            }
        };

        VEC_CTAD(vec)

    }

    #undef VEC_BASE_TYPE
    #undef VEC_DEDUCTION_GUIDE

    // Use shorthands from OpenGL
    using vec2 = vec<float, 2>;
    using vec3 = vec<float, 3>;
    using vec4 = vec<float, 4>;

};


int main() {
    math::vec x = { 1.0f, 2.0f, 3.0f };

    // details::vec_refs_kernel m = { x, 1LU, 0LU };
    // std::cout << m[1];

    // math::subvec y { x, 1, 0 };
    while (true) {
        x.slice<0, 1>().normalize();
        std::cout << x << std::endl;
        std::cout << "len: " << x.len() << std::endl;
    }

}
