#ifndef CXL_TRAITS_HPP
#define CXL_TRAITS_HPP

#pragma once

#include <type_traits>
#include <limits>
#include <cstdlib>
#include <tuple>
#include <functional>

namespace cxl {
    template<typename T>
    using unref = typename std::remove_reference<T>::type;

    template<typename T>
    using uncv = typename std::remove_cv<T>::type;

    template<typename T>
    using unrefcv = uncv<unref<T>>;

    template<typename T>
    using add_lref = T &;

    template<typename T>
    using add_rref = T &&;

    template<typename T>
    using add_const = T const;

    template<typename T>
    using add_volatile = volatile T;

    template<typename T, typename... Args>
    using result_of = decltype(std::declval<T>()(std::declval<Args>()...));

    template<bool Cond, typename T = void>
    using enable_if = typename std::enable_if<Cond, T>::type;

    template<bool Cond, typename Yes, typename No>
    using cond = typename std::conditional<Cond, Yes, No>::type;

    template<typename From, typename To>
    using copy_lref = cond<std::is_lvalue_reference<From>::value, To &, To>;

    template<typename From, typename To>
    using copy_rref = cond<std::is_lvalue_reference<From>::value, To &&, To>;

    template<typename From, typename To>
    using copy_volatile = cond<std::is_const<unref<From>>::value, volatile To, To>;

    template<typename From, typename To>
    using copy_const = cond<std::is_const<unref<From>>::value, To const, To>;

    template<typename From, typename To>
    using copy_refcv = copy_rref<From, copy_lref<From, copy_volatile<From, copy_const<From, To>>>>;

    template<bool Value>
    using bool_t = typename std::integral_constant<bool, Value>::type;

    template<std::size_t Value>
    using uint_t = typename std::integral_constant<std::size_t, Value>::type;

    template<std::size_t Base, bool... Results>
    struct get_offset;

    template<>
    struct get_offset<0> : uint_t<std::numeric_limits<std::size_t>::max()>
    {
    };

    template<std::size_t Base>
    struct get_offset<Base> : uint_t<std::numeric_limits<std::size_t>::max()>
    {
    };

    template<std::size_t Base, bool... Rest>
    struct get_offset<Base, false, Rest...> : get_offset<Base + 1, Rest...>
    {
    };

    template<std::size_t Base, bool... Rest>
    struct get_offset<Base, true, Rest...> : uint_t<Base>
    {
    };

    namespace detail {
        template<std::size_t I, typename... types>
        struct nth_type;

        template<std::size_t I>
        struct nth_type<I>
        {
        };

        template<std::size_t I, typename First, typename... Rest>
        struct nth_type<I, First, Rest...> : nth_type<I - 1, Rest...>
        {
        };

        template<typename First, typename... Rest>
        struct nth_type<0, First, Rest...>
        {
            using type = First;
        };
    } // End or namespace detail

    template<std::size_t I, typename... Types>
    using nth_type = typename detail::nth_type<I, Types...>::type;

    template<bool... Values>
    struct and_;

    template<>
    struct and_<> : bool_t<true>
    {
    };

    template<bool... Values>
    struct and_<true, Values...> : and_<Values...>
    {
    };

    template<bool... Values>
    struct and_<false, Values...> : bool_t<false>
    {
    };

    template<bool... Values>
    struct or_;

    template<>
    struct or_<> : bool_t<false>
    {
    };

    template<bool... Values>
    struct or_<false, Values...> : or_<Values...>
    {
    };

    template<bool... Values>
    struct or_<true, Values...> : bool_t<true>
    {
    };

    template<class T>
    typename std::decay<T>::type decay_copy(T &&t)
    {
        return std::forward<T>(t);
    }

    template<typename First, typename... Rest>
    struct is_same_t;

    template<typename Last>
    struct is_same_t<Last> : bool_t<true>
    {
    };

    template<typename Next, typename... Rest>
    struct is_same_t<Next, Next, Rest...> : is_same_t<Next, Rest...>
    {
    };

    template<typename First, typename Second, typename... Rest>
    struct is_same_t<First, Second, Rest...> : bool_t<false>
    {
    };

    template<typename T, typename... Types>
    struct contained_t;

    template<typename T>
    struct contained_t<T> : bool_t<false>
    {
    };

    template<typename T, typename... Types>
    struct contained_t<T, T, Types...> : bool_t<true>
    {
    };

    template<typename T, typename First, typename... Rest>
    struct contained_t<T, First, Rest...> : contained_t<T, Rest...>
    {
    };

// C++14 variable template
#if __cplusplus > 201103L
    template<typename T>
    constexpr bool is_ref = std::is_reference<T>::value;

    template<typename T>
    constexpr bool is_lref = std::is_lvalue_reference<T>::value;

    template<typename T>
    constexpr bool is_rref = std::is_rvalue_reference<T>::value;

    template<typename T>
    constexpr bool is_const = std::is_const<unref<T>>::value;

    template<typename T>
    constexpr bool is_volatile = std::is_volatile<unref<T>>::value;

    template<typename Base, typename Derived>
    constexpr bool is_base_of = std::is_base_of<Base, Derived>::value;

    template<typename To, typename... From>
    constexpr bool is_constructible = std::is_constructible<To, From...>::value;

    template<typename To, typename From>
    constexpr bool is_assignable = std::is_assignable<add_lref<To>, From>::value;

    template<bool... Values>
    constexpr bool all_of = and_<Values...>::value;

    template<bool... Values>
    constexpr bool any_of = or_<Values...>::value;

    template<typename First, typename... Rest>
    constexpr bool is_same = is_same_t<First, Rest...>::value;

    template<typename First, typename Second>
    constexpr bool is_similar = is_same_t<unrefcv<First>, Second>::value;

    template<typename T, typename... Types>
    constexpr bool contained = contained_t<T, Types...>::value;
#endif //__cplusplus > 201103L

} // End of namespace cxl

#endif // CXL_TRAITS_HPP
