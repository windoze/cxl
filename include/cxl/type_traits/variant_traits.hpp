#ifndef CXL_VARIANT_TRAITS_HPP
#define CXL_VARIANT_TRAITS_HPP

#pragma once

#include <cxl/variant/variant.hpp>

namespace cxl {
    template<typename ...T>
    struct variant;

    namespace detail {
        template<typename T, typename V>
        struct add_head;

        template<typename T, typename... V>
        struct add_head<T, variant<V...>>
        {
            using type=variant<T, V...>;
        };
    } // End of namespace cxl::detail

    template<typename T>
    struct dedup;

    template<>
    struct dedup<variant<>>
    {
        typedef variant<> type;
    };

    template<typename T>
    struct dedup<variant<T>>
    {
        typedef variant<T> type;
    };

    template<typename T, typename... Types>
    struct dedup<variant<T, Types...>>
    {
        typedef typename dedup<variant<Types...>>::type deduped_tail;
        typedef typename std::conditional<contained_t<T, Types...>::value,
                                          deduped_tail,
                                          typename detail::add_head<T, deduped_tail>::type>::type type;
    };

    /**
     * Remove duplicated types from variant, only last occurence kept
     */
    template<typename... Types>
    using deduped_variant = typename dedup<variant<Types...>>::type;
} // End of namespace cxl
#endif // CXL_VARIANT_TRAITS_HPP
