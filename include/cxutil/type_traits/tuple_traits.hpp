#ifndef CXUTIL_TUPLE_TRAITS_HPP
#define CXUTIL_TUPLE_TRAITS_HPP

#pragma once

#include <tuple>
#include <cxutil/type_traits/function_traits.hpp>

namespace utility
{
namespace detail
{
    template <typename T, typename Tp>
    struct contains;
    template <typename T, typename... Types>
    struct contains<T, std::tuple<T, Types...>>
    {
        static constexpr bool value = true;
    };
    template <typename T, typename U, typename... Types>
    struct contains<T, std::tuple<U, Types...>>
    {
        static constexpr bool value = contains<T, std::tuple<Types...>>::value;
    };
    template <typename T>
    struct contains<T, std::tuple<>>
    {
        static constexpr bool value = false;
    };
} // End of namespace utility::detail

template <typename T>
struct dedup;

template <typename T>
struct dedup<std::tuple<T>>
{
    typedef std::tuple<T> type;
};

template <typename T>
struct dedup<std::tuple<T, T>>
{
    typedef std::tuple<T> type;
};

template <typename T, typename U>
struct dedup<std::tuple<T, U>>
{
    typedef std::tuple<T, U> type;
};

template <>
struct dedup<std::tuple<>>
{
    typedef std::tuple<> type;
};

template <typename T, typename... Types>
struct dedup<std::tuple<T, Types...>>
{
    typedef typename std::conditional<detail::contains<T, std::tuple<Types...>>::value,
                                      typename dedup<std::tuple<Types...>>::type,
                                      std::tuple<T, Types...>>::type type;
};

} // End of namespace utility
#endif // CXUTIL_TUPLE_TRAITS_HPP
