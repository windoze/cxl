#ifndef CXL_STR_LIT_HPP
#define CXL_STR_LIT_HPP

////////////////////////////////////////////////////////////////////////////////////
// From Andrzej's C++ blog
// https://akrzemi1.wordpress.com/2011/05/11/parsing-strings-at-compile-time-part-i/
////////////////////////////////////////////////////////////////////////////////////

#include <stdexcept>

namespace cxl {
    namespace detail {
        constexpr unsigned in_range(unsigned i, unsigned len)
        {
            return i >= len ? throw std::out_of_range("in_range") : i;
        }

        template<unsigned N>
        constexpr char nth_char(const char(&arr)[N], unsigned i)
        {
            return in_range(i, N - 1), arr[i];
        }
    } // End of namespace detail

    class str_lit
    {
        char *const begin_;
        unsigned size_;

    public:
        template<unsigned N>
        constexpr str_lit(const char(&arr)[N])
                : begin_(arr), size_(N - 1)
        {
            static_assert(N >= 1, "not a string literal");
        }

        constexpr char operator[](unsigned i) const { return detail::in_range(i, size_), begin_[i]; }

        constexpr operator const char *() const { return begin_; }

        constexpr unsigned size() const { return size_; }
    };

    inline constexpr unsigned count(str_lit str, char c, unsigned i = 0, unsigned ans = 0)
    {
        return i == str.size() ? ans : str[i] == c ? count(str, c, i + 1, ans + 1)
                                                   : count(str, c, i + 1, ans);
    }

    namespace detail {
        inline constexpr int
        balanced_impl(str_lit str, char left, char right, unsigned i = 0, int ans = 0)
        {
            return i == str.size() ? ans : str[i] == left
                                           ? balanced_impl(str, i + 1, ans + 1)
                                           : str[i] == right ? balanced_impl(str, i + 1, ans - 1)
                                                             : balanced_impl(str, i + 1, ans);
        }
    }

    inline constexpr bool balanced(str_lit str, char left, char right)
    {
        return detail::balanced_impl(str, left, right) == 0;
    }

} // End of namespace cxl

#endif
