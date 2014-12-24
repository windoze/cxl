#pragma once

#include <ostream>

namespace utility
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
}   // End of namespace utility::detail

template <typename First, typename... Rest>
std::ostream&
operator<<(std::ostream& out,
           variant<First, Rest...> const& v) // http://stackoverflow.com/questions/23355117/
{
    detail::printer const printer_{out};
    return v.apply_visitor(printer_);
}
}   // End of namespace utility
