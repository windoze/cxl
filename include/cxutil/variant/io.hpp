#ifndef CXUTIL_VARIANT_IO_HPP
#define CXUTIL_VARIANT_IO_HPP

#pragma once

#include <ostream>
#include <tuple>
#include <cxutil/type_traits/tuple_traits.hpp>
#include <cxutil/variant/variant.hpp>

namespace cxutil
{
namespace detail
{
    struct printer
    {
        template <typename type>
        std::ostream& operator()(type const& value) const
        {
            return out << value;
        }

        std::ostream& out;
    };

    template <typename Src, typename T>
    struct variant_reader;

    template <typename Src, typename... Types>
    struct variant_reader<Src, std::tuple<Types...>>
    {
        typedef std::tuple<Types...> type_tuple;

        template <typename V>
        struct read_null
        {
            static void invoke(Src&, int, V&) { /* Do nothing */}
        };

        template <typename V>
        struct read_impl
        {
            static void invoke(Src& src, int which, V& v)
            {
                if (which == 0) {
                    typedef typename tuple_head<type_tuple>::type head_type;
                    head_type value;
                    src >> value;
                    v = value;
                }
                variant_reader<Src, typename tuple_tail<type_tuple>::type>::read(src, which - 1, v);
            }
        };

        template <typename V>
        static void read(Src& src, int which, V& v)
        {
            std::conditional<tuple_empty<type_tuple>::value,
                             read_null<V>,
                             read_impl<V>>::type::invoke(src, which, v);
        }
    };

    template <typename Sink>
    struct variant_writer
    {
        variant_writer(Sink& sink) : sink_(sink) {}
        template <class T>
        void operator()(T const& value) const
        {
            sink_ << value;
        }
        Sink& sink_;
    };
} // End of namespace cxutil::detail

/**
 * Stream output
 * @see http://stackoverflow.com/questions/23355117/
 */
template <typename First, typename... Rest>
std::ostream& operator<<(std::ostream& out, variant<First, Rest...> const& v)
{
    detail::printer const printer{out};
    return v.apply_visitor(printer);
}

/**
 * For serialization and deserialization, i.e. Boost.Serialization
 */
template <typename Src, typename... Types>
Src& read(Src& src, variant<Types...>& v)
{
    decltype(v.which()) which;
    src >> which;
    if (which >= sizeof...(Types)) {
        // this might happen if a type was removed from the list of variant types
        throw std::runtime_error("Invalid data");
    }
    detail::variant_reader<Src, std::tuple<Types...>>::read(src, which, v);
    return src;
}

template <typename Sink, typename... Types>
Sink& write(Sink& sink, const variant<Types...>& v)
{
    sink << v.which();
    detail::variant_writer<Sink> visitor(sink);
    v.apply_visitor(visitor);
    return sink;
}
} // End of namespace cxutil

#endif // CXUTIL_VARIANT_IO_HPP
