#ifndef CXUTIL_VARIANT_COMPARE_HPP
#define CXUTIL_VARIANT_COMPARE_HPP

#pragma once

#include <cxutil/variant/variant.hpp>
#include <cxutil/variant/visitor.hpp>

namespace cxutil
{
namespace detail
{
    struct equality_comparator
    {
        template <typename T>
        bool operator()(T const& lhs, T const& rhs) const
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
            return static_cast<bool>(lhs == rhs);
#pragma clang diagnostic pop
        }

        template <typename L, typename R>
        bool operator()(L const&, R const&) const
        {
            return false;
        }
    };
}   // End of namespace cxutil::detail

template <typename... lhs_types, typename... rhs_types>
bool operator==(variant<lhs_types...> const& lhs, variant<rhs_types...> const& rhs)
{
    detail::equality_comparator equality_comparator;
    return apply_visitor(equality_comparator, lhs, rhs);
}

namespace detail
{
    template <typename VariantType>
    struct less_than_comparator
    {
        template <typename T>
        bool operator()(T const& lhs, T const& rhs) const
        {
            return static_cast<bool>(lhs < rhs);
        }

        template <typename L, typename R>
        bool operator()(L const&, R const&) const
        {
            return (which_bounded_type<VariantType, L> < which_bounded_type<VariantType, R>);
        }
    };
}   // End of namespace cxutil::detail

template <typename... Types>
bool operator<(variant<Types...> const& lhs, variant<Types...> const& rhs)
{
    detail::less_than_comparator<variant<Types...>> less_than_comparator;
    return apply_visitor(less_than_comparator, lhs, rhs);
}
}   // End of namespace cxutil

#endif  // CXUTIL_VARIANT_COMPARE_HPP
