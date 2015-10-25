#ifndef CXL_VARIANT_VISITOR_HPP
#define CXL_VARIANT_VISITOR_HPP

#pragma once

#include <utility>
#include <cxl/variant/variant.hpp>

namespace cxl {
    namespace detail {
        template<typename T, bool = is_variant<unrefcv<T>>::value>
        struct underlying_type;

        template<typename Visitable>
        struct underlying_type<Visitable, true>
        {
            using type = copy_refcv<Visitable &&, typename unref<Visitable>::template type<0>>;
        };

        template<typename Nonvisitable>
        struct underlying_type<Nonvisitable, false>
        {
            using type = Nonvisitable;
        };

        template<typename Visitable>
        using equivalent_type = typename underlying_type<Visitable &&>::type;

        template<typename Ret, typename Supervisitor, typename T, bool = is_variant<unrefcv<T>>::value>
        struct subvisitor;

        template<typename Ret, typename Supervisitor, typename Visitable>
        struct subvisitor<Ret, Supervisitor, Visitable, true>
        { // visitation
            template<typename... Visited>
            Ret operator()(Visited &&... visited) const
            {
                return std::forward<Visitable>(visitable_)
                        .apply_visitor(std::forward<Supervisitor>(supervisitor_),
                                       std::forward<Visited>(visited)...);
            }

            Supervisitor &&supervisitor_;
            Visitable &&visitable_;
        };

        template<typename Ret, typename Supervisitor, typename T>
        struct subvisitor<Ret, Supervisitor, T, false>
        { // forwarding

            template<typename... Visited>
            Ret operator()(Visited &&... visited) const
            {
                return std::forward<Supervisitor>(supervisitor_)(std::forward<T>(value_),
                                                                 std::forward<Visited>(visited)...);
            }

            Supervisitor &&supervisitor_;
            T &&value_;
        };

        template<typename Ret, typename... Visitables>
        struct visitor_partially_applier;

        template<typename Ret>
        struct visitor_partially_applier<Ret>
        { // backward
            template<typename Visitor>
            Ret operator()(Visitor &&visitor) const
            {
                return std::forward<Visitor>(visitor)();
            }
        };

        template<typename Ret, typename First, typename... Rest>
        struct visitor_partially_applier<Ret, First, Rest...>
                : visitor_partially_applier<Ret, Rest...>
        { // forward
            using base = visitor_partially_applier<Ret, Rest...> const;

            template<typename Visitor>
            Ret operator()(Visitor &&visitor, First &&first, Rest &&... rest) const
            {
                subvisitor<Ret, Visitor &&, First &&> const subvisitor{std::forward<Visitor>(visitor),
                                                                       std::forward<First>(first)};
                return base::operator()(subvisitor, std::forward<Rest>(rest)...);
            }
        };
    } // End of namespace cxl::detail

    template<typename Visitor, typename First, typename... Rest>
    result_of<Visitor &&, detail::equivalent_type<First &&>, detail::equivalent_type<Rest &&>...>
    apply_visitor(Visitor &&visitor, First &&first, Rest &&... rest)
    {
        using result_type = result_of<Visitor &&,
                                      detail::equivalent_type<First &&>,
                                      detail::equivalent_type<Rest &&>...>;
        detail::visitor_partially_applier<result_type, First &&, Rest &&...> apply_visitor_partially;
        return apply_visitor_partially(
                std::forward<Visitor>(visitor), std::forward<First>(first), std::forward<Rest>(rest)...);
    }

    namespace detail {
        template<typename Visitor>
        struct delayed_visitor_applier
        {
            static_assert(!std::is_rvalue_reference<Visitor>::value, "xx");

            constexpr delayed_visitor_applier(Visitor &&visitor) noexcept
                    : visitor_(std::forward<Visitor>(visitor))
            {
                ;
            }

            template<typename Visitable, typename = enable_if<is_variant<unrefcv<Visitable>>::value>>
            auto
            operator()(Visitable &&visitable) const -> decltype(std::forward<Visitable>(visitable)
                    .apply_visitor(this->visitor_))
            {
                return std::forward<Visitable>(visitable).apply_visitor(visitor_);
            }

            template<typename Visitable, typename = enable_if<is_variant<unrefcv<Visitable>>::value>>
            auto operator()(Visitable &&visitable)
            -> decltype(std::forward<Visitable>(visitable).apply_visitor(this->visitor_))
            {
                return std::forward<Visitable>(visitable).apply_visitor(visitor_);
            }

            template<typename... Visitables>
            auto operator()(Visitables &&... visitables) const
            -> decltype(apply_visitor(this->visitor_, std::forward<Visitables>(visitables)...))
            {
                return apply_visitor(visitor_, std::forward<Visitables>(visitables)...);
            }

            template<typename... Visitables>
            auto operator()(Visitables &&... visitables)
            -> decltype(apply_visitor(this->visitor_, std::forward<Visitables>(visitables)...))
            {
                return apply_visitor(visitor_, std::forward<Visitables>(visitables)...);
            }

        private:
            Visitor visitor_;
        };
    } // End of namespace cxl::detail

    template<typename Visitor>
    constexpr detail::delayed_visitor_applier<Visitor> apply_visitor(Visitor &&visitor) noexcept
    {
        return std::forward<Visitor>(visitor);
    }
} // End of namespace cxl

#endif // CXL_VARIANT_VISITOR_HPP
