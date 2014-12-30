#ifndef CXUTIL_TUPLE_TRAITS_HPP
#define CXUTIL_TUPLE_TRAITS_HPP

#pragma once

#include <tuple>
#include <type_traits>

namespace cxutil
{
/**
 * type_index retrieves the first index of type T from Tuple
 */
template <typename T, typename Tuple>
struct type_index;

template <typename T, typename... Types>
struct type_index<T, std::tuple<T, Types...>> {
    static constexpr std::size_t value = 0;
};

template <typename T, typename U, typename... Types>
struct type_index<T, std::tuple<U, Types...>> {
    static constexpr std::size_t value = 1 + type_index<T, std::tuple<Types...>>::value;
};

// C++14 variable template
#if __cplusplus > 201103L
template <typename T, typename Tuple>
constexpr std::size_t typeindex = type_index<T, Tuple>::value;
#endif

template <typename T>
struct tuple_head;

template <typename T, typename... Types>
struct tuple_head<std::tuple<T, Types...>> {
    typedef T type;
};

template <typename T>
struct tuple_tail;

template <typename T, typename... Types>
struct tuple_tail<std::tuple<T, Types...>> {
    typedef std::tuple<Types...> type;
};

template <typename T>
struct tuple_empty;

template <typename... Types>
struct tuple_empty<std::tuple<Types...>> {
    static constexpr bool value = false;
};

template <>
struct tuple_empty<std::tuple<>> {
    static constexpr bool value = true;
};

namespace detail
{
    template <typename T, typename V>
    struct add_head;

    template <typename T, typename... Types>
    struct add_head<T, std::tuple<Types...>> {
        typedef std::tuple<T, Types...> type;
    };
} // End of namespace cxutil::detail

template <typename T>
struct dedup;

template <>
struct dedup<std::tuple<>> {
    typedef std::tuple<> type;
};

template <typename T>
struct dedup<std::tuple<T>> {
    typedef std::tuple<T> type;
};

template <typename T, typename... Types>
struct dedup<std::tuple<T, Types...>> {
    typedef typename dedup<std::tuple<Types...>>::type deduped_tail;
    typedef typename std::conditional<contained_t<T, Types...>::value,
                                      deduped_tail,
                                      typename detail::add_head<T, deduped_tail>::type>::type type;
};

} // End of namespace cxutil
#endif // CXUTIL_TUPLE_TRAITS_HPP
