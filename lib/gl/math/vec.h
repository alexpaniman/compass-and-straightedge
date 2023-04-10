#pragma once

#include <cmath>
#include <concepts>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
#include <ostream>
#include <type_traits>
#include <tuple>
#include <utility>

namespace details {

    template <typename value_type, typename callback_type>
    class catch_modifications_proxy {
    public:
        catch_modifications_proxy(value_type& editable_value, callback_type&& callback)
            : m_editable_value(editable_value), m_callback(callback) {}

        #define FORWARD_OPERATOR(name)                                          \
            template <typename other_value>                                     \
            catch_modifications_proxy& operator name(other_value&& value) {     \
                m_callback(); /* Notify about assignment */                     \
                m_editable_value name std::forward<other_value>(value);         \
                return *this;                                                   \
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
        operator const value_type&() const { return m_editable_value; }

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
        constexpr random_access_iterator_proxy(indexable_type& indexable, std::size_t starting_index)
            : m_indexable(indexable), m_current_index(starting_index) {}

        constexpr random_access_iterator_proxy<indexable_type>& operator++() {
            ++ m_current_index;
            return *this;
        }

        constexpr decltype(auto) operator*() { return m_indexable[m_current_index]; }

        constexpr bool operator!=(const random_access_iterator_proxy<indexable_type>& other) const {
            return m_current_index != other.m_current_index;
        }

    private:
        indexable_type& m_indexable;
        std::size_t m_current_index;
    };

    template <typename indexable_type>
    class const_random_access_iterator_proxy {
    public:
        constexpr const_random_access_iterator_proxy(const indexable_type& indexable, std::size_t starting_index)
            : m_indexable(indexable), m_current_index(starting_index) {}

        constexpr const_random_access_iterator_proxy<indexable_type>& operator++() {
            ++ m_current_index;
            return *this;
        }

        constexpr decltype(auto) operator*() const { return m_indexable[m_current_index]; }

        constexpr bool operator!=(const const_random_access_iterator_proxy<indexable_type>& other) const {
            return m_current_index != other.m_current_index;
        }

    private:
        const indexable_type& m_indexable;
        std::size_t m_current_index;
    };


    template <typename impl_type, typename element_type, std::size_t count>
    class vec_base_kernel {
    public:
        template<typename... vector_coordinates>
        constexpr vec_base_kernel(vector_coordinates... initializer_coordinates)
            : m_coordinates { initializer_coordinates... } {

            static_assert(sizeof...(vector_coordinates) == count,
                          "Invalid number of vector coordinates!");
        }

        constexpr auto operator[](const std::size_t index) {
            return catch_modifications_proxy(m_coordinates[index],
                    static_cast<impl_type*>(this)->get_change_callback());
        }

        constexpr const auto& operator[](const std::size_t index) const {
            return m_coordinates[index];
        }

    private:
        element_type  m_coordinates[count];
    };

    template <typename impl_type, typename element_type, std::size_t count>
    class vec_refs_kernel {
    public:
        template<typename... index_types>
        constexpr vec_refs_kernel(impl_type& original_vec, index_types... indices)
            : m_original_vec(original_vec),
              m_index_translation_map { static_cast<std::size_t>(indices)... } {

            static_assert(sizeof...(indices) == count,
                          "Invalid number of vector coordinates!");
        }

        constexpr decltype(auto) operator[](const std::size_t index) {
            return m_original_vec[m_index_translation_map[index]];
        }

        constexpr decltype(auto) operator[](const std::size_t index) const {
            return m_original_vec[m_index_translation_map[index]];
        }

    private:
        impl_type& m_original_vec;
        std::size_t m_index_translation_map[count];
    };

    // TODO: Do i need this?
    // template<typename impl_type, typename... index_types>                                    
    // vec_refs_kernel(impl_type& original_vec, index_types... indices) ->                    
    //     vec_refs_kernel<impl_type, decltype(original_vec[0]), sizeof...(indices)>;


    // If vector stores floating point values, use the same value for len type,
    // on the other hand, if it uses anything else fallback to double.
    template <typename element_type>
    using default_len_type =
        std::conditional_t<std::is_floating_point_v<element_type>, element_type, double>;

    template <typename vec_type, typename element_type>
    concept has_coordinates = requires(vec_type vec, std::size_t i) {
        { vec[i] } -> std::convertible_to<element_type>;
    };

    template <typename vec_type, typename element_type>
    concept right_multipliable = requires(vec_type vec, element_type value) {
        { vec * value };
    };

    template <typename type>
    concept convertible_to_long_double = std::convertible_to<type, long double>;

    template <typename type>
    concept convertible_to_long_long = std::convertible_to<type, long long>;

    template <typename type>
    concept convertible_to_numeric =
        convertible_to_long_double<type> || convertible_to_long_long<type>;

    //        for CRTP            for switching kernels
    template <typename impl_type, typename vec_kernel_impl_type, 
              template <class...> typename vec_sliced_impl_type,  // for instantiation of sliced vec
              typename element_type, std::size_t count, typename derived_type>
    class vec_shell_base: public vec_kernel_impl_type {
    public:
        using vec_kernel_impl_type::vec_kernel_impl_type;

        // => The only way to interact with vector's components
        using vec_kernel_impl_type::operator[];

        // =======> 

        using value_type = element_type;
        static constexpr std::size_t size() { return count; }

        // =======> Implement convenient coordinate getters for our vectors: <=======

        // Make sure there is no vec.z() in two-dimensional vec (which has only x and y)
        #define STATIC_ASSERT_AVAILABILITY(coordinate_name, index)                 \
            static_assert(count > index, "Vector doesn't have `" #coordinate_name  \
                                         "' because it's too small!");

        #define COORDINATE_GETTER(coordinate_name, index)                          \
            constexpr decltype(auto) coordinate_name() {                           \
                STATIC_ASSERT_AVAILABILITY(coordinate_name, index)                 \
                return vec_kernel_impl_type::operator[](index);                    \
            }                                                                      \
                                                                                   \
            constexpr const element_type &coordinate_name() const {                \
                STATIC_ASSERT_AVAILABILITY(coordinate_name, index)                 \
                return vec_kernel_impl_type::operator[](index);                    \
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
                for (std::size_t i = 0; i < count; ++ i)                           \
                    vec_kernel_impl_type::operator[](i) assignment other[i];       \
                                                                                   \
                return *get_impl(); /* CRTP Polymorphic chaining */                \
            }

        DEFINE_ASSIGNMENT(*=) DEFINE_ASSIGNMENT(/=)
        DEFINE_ASSIGNMENT(-=) DEFINE_ASSIGNMENT(+=)

        #undef DEFINE_ASSIGNMENT


        // =======> Implement basic coordinatewise operations on vectors: <=======

        #define DEFINE_OPERATOR(name, corresponding_assignment)                    \
            template <has_coordinates<element_type> other_vec>                     \
            constexpr impl_type operator name(const other_vec& other) const {      \
                impl_type new_vec = *get_impl();                                   \
                return new_vec corresponding_assignment other;                     \
            }

        DEFINE_OPERATOR(*, *=) DEFINE_OPERATOR(-, -=)
        DEFINE_OPERATOR(+, +=) DEFINE_OPERATOR(/, /=)

        #undef DEFINE_OPERATOR

        // =======> Multiplication and division by value:

        #define DEFINE_BY_VALUE_OPERATOR(name, assignment)                          \
            template <convertible_to_numeric value_type>                            \
            constexpr impl_type& operator assignment(const value_type value) {      \
                for (std::size_t i = 0; i < count; ++ i)                            \
                    vec_kernel_impl_type::operator[](i) assignment value;           \
                                                                                    \
                return *get_impl();                                                 \
            }                                                                       \
                                                                                    \
            template <convertible_to_numeric value_type>                            \
            constexpr impl_type operator name(const value_type value) const {       \
                impl_type new_vec = *get_impl();                                    \
                return new_vec assignment value;                                    \
            }

        DEFINE_BY_VALUE_OPERATOR(*, *=) DEFINE_BY_VALUE_OPERATOR(/, /=)
                                                                                    
        #undef DEFINE_BY_VALUE_OPERATOR 

        #define TWO_COORDINATE_SLICE(x, y, id_x, id_y)                              \
            constexpr decltype(auto) x##y() { return slice(id_x, id_y); }

        TWO_COORDINATE_SLICE(x, y, 0, 1)
        TWO_COORDINATE_SLICE(y, x, 1, 0)
        TWO_COORDINATE_SLICE(y, z, 1, 2)
        TWO_COORDINATE_SLICE(z, y, 2, 1)
        TWO_COORDINATE_SLICE(x, z, 0, 2)
        TWO_COORDINATE_SLICE(z, x, 0, 2)

        #undef TWO_COORDINATE_SLICE

        // =======> Left hand side multiplication by value:

        template <convertible_to_numeric value_type>
        friend constexpr impl_type operator*(const value_type value,
                                             const impl_type& other) {
            return other * value;
        }

        constexpr random_access_iterator_proxy<impl_type>       begin()       { return { *get_impl(),     0 }; }
        constexpr random_access_iterator_proxy<impl_type>         end()       { return { *get_impl(), count }; }

        constexpr const_random_access_iterator_proxy<impl_type> begin() const { return { *get_impl(),     0 }; }
        constexpr const_random_access_iterator_proxy<impl_type>   end() const { return { *get_impl(), count }; }

        template <typename other_vector>
        constexpr element_type dot(const other_vector& other) const {
            element_type accumulator {};
            for (std::size_t i = 0; i < count; ++ i)
                accumulator += (*this)[i] * other[i];

            return accumulator;
        }

        template <typename other_vector>
        constexpr impl_type cross(const other_vector& other) {
            static_assert(count == 3, "Cross product is only defined for 3D case.");

            return impl_type {
                y() * other.x() - z() * other.y(),
                x() * other.z() - z() * other.x(),
                x() * other.y() - y() * other.x()
            };
        }

        constexpr derived_type len() const {
            return static_cast<derived_type>(sqrt(this->dot(*this)));
        }

        constexpr impl_type& normalize() {
            return (*this) *= (static_cast<derived_type>(1) / len());
        }

        constexpr impl_type& rotate(double angle) {
            static_assert(count == 2, "Rotation for now is impemented only for 2D case!");

            double new_x = static_cast<element_type>(cos(angle) * x() - sin(angle) * y());
            y()          = static_cast<element_type>(sin(angle) * x() + cos(angle) * y());

            x() = new_x;

            return *get_impl();
        }

        template <typename other_vector>
        constexpr impl_type& rotate(const other_vector& pivot, double angle) {
            (*this) -= pivot;
            rotate(angle);
            (*this) += pivot;

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

        template <typename other_vector>
        constexpr impl_type rotated(const other_vector &pivot, double angle) const {
            impl_type new_vec = *get_impl();
            return new_vec.rotate(pivot, angle);
        }

        constexpr impl_type normalized() const {
            impl_type new_vec = *get_impl();
            return new_vec.normalize();
        }


        friend std::ostream& operator<<(std::ostream& os, const impl_type& vector) {
            os << "(";

            for (std::size_t i = 0; i < count; ++ i) {
                os << vector[i];
                if (i != count - 1) os << ", ";
            }

            return os << ")";
        }

        template <typename... index_types>
        auto slice(index_types ...indicies) {
            return vec_sliced_impl_type { *get_impl(), indicies... };
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
    #define GENERIC_VEC_BASE_TYPE(impl, kernel, count)                                          \
        details::vec_shell_base<impl, kernel, subvec, element_type, count, derived_type>

    #define _ ,
    #define SUBVEC_BASE_TYPE                                                                    \
        GENERIC_VEC_BASE_TYPE(subvec<vec_impl_type _ element_type _ wrapped_count _ derived_type>, \
            details::vec_refs_kernel<vec_impl_type _ element_type _ wrapped_count::value>, wrapped_count::value) \
        // details::vec_shell_base<                                                 \
        //     subvec<vec_impl_type, element_type, wrapped_count, derived_type>,     \
        //     details::vec_refs_kernel<vec_impl_type, element_type, wrapped_count::value>, \
        //     subvec, element_type, wrapped_count::value, derived_type>

    template <typename vec_impl_type, typename element_type,
              typename wrapped_count, typename derived_type>
    class subvec: public SUBVEC_BASE_TYPE {
    public:
        using SUBVEC_BASE_TYPE::vec_shell_base;
    };

    template<template <typename element_type, std::size_t count, typename len_type>
             typename captured_vec_type, // templated referenced vec
             //                                                          v~~  linked index types
             typename element_type, std::size_t count, typename derived_type, typename... index_types>                                    
             //       ^~~~~~~~~~~~         ^~~~~           ^~~~~~~~~~~~ extract vec's template args 
    subvec(captured_vec_type<element_type, count, derived_type>& original_vec, index_types... indices) ->                    
        subvec<captured_vec_type<element_type, count, derived_type>, element_type,
               std::integral_constant<std::size_t, sizeof...(indices)>, derived_type>;

    // Deduction guide has to be defined twice (for both vecs), but they
    // have the exact same interface, so we'll use macro (there is no other way)

    #define VEC_CTAD(vec)                                                        \
        template<class... vector_coordinates>                                    \
        vec(vector_coordinates... initializer_coordinates) ->                    \
            vec<details::strip_proxy_t<                                          \
                    std::tuple_element_t<0, std::tuple<vector_coordinates...>>>, \
                sizeof...(vector_coordinates)>; // Deduce vec type by first element!

    #define VEC_BASE_TYPE                                                                         \
        GENERIC_VEC_BASE_TYPE(vec<element_type _ count _ derived_type>,                           \
                              details::vec_base_kernel<                                           \
                                vec<element_type _ count _ derived_type> _ element_type _ count>, \
                              count)

    inline // Makes uncached::vec "leak" in outer namespace, making it "default" in a way
    namespace uncached {

        template <typename element_type, std::size_t count,
                  typename derived_type = details::default_len_type<element_type>>
        class vec: public VEC_BASE_TYPE {
        public:
            using VEC_BASE_TYPE::vec_shell_base;
        };

        VEC_CTAD(vec)

    }

    namespace cached {

        template <typename element_type, std::size_t count,
                  typename derived_type = details::default_len_type<element_type>>
        class vec: public VEC_BASE_TYPE {
        public:
            using VEC_BASE_TYPE::vec_shell_base;

            constexpr derived_type len() const /* CRTP override */ {
                if (!std::isnan(m_cached_length))
                    return m_cached_length;

                return m_cached_length = VEC_BASE_TYPE::len();
            }

        private:
            mutable derived_type m_cached_length = std::numeric_limits<derived_type>::quiet_NaN();

            // Do not expose notify_vector_changed, yet override it!
            friend class VEC_BASE_TYPE;

            void notify_vector_changed() /* CRTP override */ {
                // Invalidate current length, it will be recalculated on demand!
                m_cached_length = std::numeric_limits<derived_type>::quiet_NaN();
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

    struct proxy_zero_vec {
        template <typename element_type, std::size_t count>
        constexpr operator math::vec<element_type, count>() const { // TODO: support any vector
            return generate<element_type, count>(std::make_index_sequence<count>());
        }

    private:
        template <typename element_type, std::size_t zero, std::size_t... indices>
        static constexpr math::vec<element_type, zero> generate(std::index_sequence<indices...>) {
            return { default_construct<element_type>(indices)... };
        }

        template <typename element_type>
        static constexpr element_type default_construct(std::size_t) { return {}; }
    };

    inline constexpr proxy_zero_vec zero = {};

    struct proxy_random_vec {
        template <typename element_type, std::size_t count>
        constexpr operator math::vec<element_type, count>() const {
            return generate<element_type, count>(std::make_index_sequence<count>());
        }

    private:
        template <typename element_type, std::size_t zero, std::size_t... indices>
        static constexpr math::vec<element_type, zero> generate(std::index_sequence<indices...>) {
            return { construct_random<element_type>(indices)... };
        }

        template <std::floating_point element_type>
        static constexpr element_type construct_random(std::size_t) {
            return static_cast<double>(rand()) / RAND_MAX;
        }

        template <std::integral element_type>
        static constexpr element_type construct_random(std::size_t) {
            return rand();
        }
    };

    inline constexpr proxy_random_vec random = {};

};
