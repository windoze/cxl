#ifndef CXL_REFLECTION_CSV_HPP
#define CXL_REFLECTION_CSV_HPP

#include <cstddef>
#include <type_traits>
#include <string>
#include <cxl/variant.hpp>
#include <cxl/reflection/reflection_impl.hpp>

namespace cxl {
    namespace reflection {
        namespace csv {
            namespace detail {
                inline char to_hex(char x)
                {
                    unsigned char c = static_cast<unsigned char>(x);
                    if (c >= 0 && c <= 9)
                        return static_cast<char>(c + '0');
                    if (c >= 10 && c <= 15)
                        return static_cast<char>(c - 10 + 'A');
                    return ' ';
                }

                template<typename OutputIterator>
                std::size_t escape(char c, OutputIterator &&i)
                {
                    if (c == '"' || c == '\t' || c == '\r' || c == '\n') {
                        (*i++) = '\\';
                        (*i++) = c;
                        return 2;
                    } else if (static_cast<unsigned char>(c) < 32 /* UTF-8: || c > 127*/) {
                        (*i++) = '\\';
                        (*i++) = 'x';
                        (*i++) = to_hex(static_cast<unsigned char>(c) >> 4);
                        (*i++) = to_hex(static_cast<unsigned char>(c) & 0x0F);
                        return 4;
                    } else {
                        (*i++) = c;
                        return 1;
                    }
                }

                template<typename Traits=std::char_traits<char>,
                         typename Allocator = std::allocator<char>,
                         typename OutputIterator>
                std::size_t write_impl(const std::basic_string<char, Traits, Allocator> &s, OutputIterator &i)
                {
                    std::size_t ret = 2;
                    (*i++) = '"';
                    for (char c: s) {
                        ret += escape(c, std::forward<OutputIterator>(i));
                    }
                    (*i++) = '"';
                    return ret;
                }

                template<typename OutputIterator>
                std::size_t write_impl(const char *str, OutputIterator &&i)
                {
                    std::size_t ret = 2;
                    (*i++) = '"';
                    while (char c = (*str++)) {
                        ret += escape(c, std::forward<OutputIterator>(i));
                    }
                    (*i++) = '"';
                    return ret;
                }

                // TODO: Optimization
                template<typename T, typename OutputIterator>
                std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, std::size_t>
                write_impl(T t, OutputIterator &&i)
                {
                    std::string s = std::to_string(t);
                    for (char c: s) {
                        (*i++) = c;
                    }
                    return s.size();
                }

                template<typename OutputIterator>
                struct write_visitor
                {
                    write_visitor(OutputIterator &&i) : oi(i) { }

                    template<typename U>
                    std::size_t operator()(const U &t) const
                    {
                        return write_impl(t, oi);
                    }

                    OutputIterator &oi;
                };

                template<typename ...T, typename OutputIterator>
                std::size_t write_impl(const variant<T...> &t, OutputIterator &&i)
                {
                    return t.apply_visitor(write_visitor<OutputIterator>(std::forward<OutputIterator>(i)));
                }
            }   // End of namespace detail

            template<typename T, typename OutputIterator>
            std::enable_if_t<cxl::reflectable<T>, std::size_t> write(const T &t, OutputIterator &&i)
            {
                std::size_t ret = cxl::tuple_size<T>::value;    // N-1 commas and 1 '\n'
                for (std::size_t n = 0; n < cxl::tuple_size<T>::value; n++) {
                    if (n > 0) (*i++) = ',';
                    ret += detail::write_impl(cxl::get_variant<T>(n, t), std::forward<OutputIterator>(i));
                }
                (*i++) = '\n';
                return ret;
            }

            template<typename T, typename OutputIterator>
            std::enable_if_t<cxl::reflectable<T>, std::size_t> write_header(OutputIterator &&i)
            {
                std::size_t ret = cxl::tuple_size<T>::value;    // N-1 commas and 1 '\n'
                for (std::size_t n = 0; n < cxl::tuple_size<T>::value; n++) {
                    if (n > 0) (*i++) = ',';
                    ret += detail::write_impl(cxl::get_element_csv_field<T>(n), std::forward<OutputIterator>(i));
                }
                (*i++) = '\n';
                return ret;
            }

            template<typename InputIterator, typename OutputIterator>
            std::size_t write_csv(InputIterator begin, InputIterator end, OutputIterator &&oi, bool with_header = true)
            {
                using value_type=typename std::iterator_traits<InputIterator>::value_type;
                static_assert(cxl::reflectable<value_type>, "Value type must be reflectable");
                std::size_t ret = 0;
                if (with_header) {
                    ret = write_header<value_type>(std::forward<OutputIterator>(oi));
                }
                for (InputIterator i = begin; i != end; ++i) {
                    ret += write(*i, std::forward<OutputIterator>(oi));
                }
                return ret;
            }
        }
    }
}

#endif // CXL_REFLECTION_CSV_HPP
