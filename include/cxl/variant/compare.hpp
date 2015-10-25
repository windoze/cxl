#ifndef CXL_VARIANT_COMPARE_HPP
#define CXL_VARIANT_COMPARE_HPP

#pragma once

#include <cxl/variant/variant.hpp>
#include <cxl/variant/visitor.hpp>

namespace cxl {
    namespace detail {
        struct equality_comparator
        {
            template<typename T>
            bool operator()(T const &lhs, T const &rhs) const
            {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
                return static_cast<bool>(lhs == rhs);
#pragma clang diagnostic pop
            }

            template<typename L, typename R>
            bool operator()(L const &, R const &) const
            {
                return false;
            }
        };
    } // End of namespace cxl::detail

    template<typename... lhs_types, typename... rhs_types>
    bool operator==(variant<lhs_types...> const &lhs, variant<rhs_types...> const &rhs)
    {
        detail::equality_comparator equality_comparator;
        return apply_visitor(equality_comparator, lhs, rhs);
    }

    namespace detail {
        template<typename VariantType>
        struct less_than_comparator
        {
            template<typename T>
            bool operator()(T const &lhs, T const &rhs) const
            {
                return static_cast<bool>(lhs < rhs);
            }

            template<typename L, typename R>
            bool operator()(L const &, R const &) const
            {
                return (VariantType::template which_type<L>::value) < (VariantType::template which_type<R>::value);
            }
        };
    } // End of namespace cxl::detail

    template<typename... Types>
    bool operator<(variant<Types...> const &lhs, variant<Types...> const &rhs)
    {
        detail::less_than_comparator<variant<Types...>> less_than_comparator;
        return apply_visitor(less_than_comparator, lhs, rhs);
    }
} // End of namespace cxl

#endif // CXL_VARIANT_COMPARE_HPP
