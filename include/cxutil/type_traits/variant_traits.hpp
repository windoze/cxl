#ifndef CXUTIL_VARIANT_TRAITS_HPP
#define CXUTIL_VARIANT_TRAITS_HPP

#pragma once

#include <tuple>
#include <cxutil/variant/variant.hpp>

namespace cxutil
{
namespace detail
{
    template <typename T, typename Tp>
    struct contains;
    template <typename T, typename... Types>
    struct contains<T, variant<T, Types...>>
    {
        static constexpr bool value = true;
    };
    template <typename T, typename U, typename... Types>
    struct contains<T, variant<U, Types...>>
    {
        static constexpr bool value = contains<T, variant<Types...>>::value;
    };
    template <typename T>
    struct contains<T, variant<>>
    {
        static constexpr bool value = false;
    };
} // End of namespace cxutil::detail

template <typename T>
struct dedup;

template <typename T>
struct dedup<variant<T>>
{
    typedef variant<T> type;
};

template <typename T>
struct dedup<variant<T, T>>
{
    typedef variant<T> type;
};

template <typename T, typename U>
struct dedup<variant<T, U>>
{
    typedef variant<T, U> type;
};

template <>
struct dedup<variant<>>
{
    typedef variant<> type;
};

template <typename T, typename... Types>
struct dedup<variant<T, Types...>>
{
    typedef typename std::conditional<detail::contains<T, std::tuple<Types...>>::value,
                                      typename dedup<std::tuple<Types...>>::type,
                                      std::tuple<T, Types...>>::type type;
};

} // End of namespace cxutil
#endif // CXUTIL_VARIANT_TRAITS_HPP
