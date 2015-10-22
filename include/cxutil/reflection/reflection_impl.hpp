#ifndef CXUTIL_REFLECTION_REFLECTION_IMPL_HPP
#define CXUTIL_REFLECTION_REFLECTION_IMPL_HPP

#include <string>
#include <cxutil/variant.hpp>

namespace cxutil
{
namespace reflection
{
    template <std::size_t I, typename T>
    struct reflected_element;

    template <std::size_t I, typename... T>
    struct reflected_element<I, std::tuple<T...>> {
        static constexpr const char* name = "";
        static constexpr const char* key = "";
        using type = typename std::tuple_element<I, std::tuple<T...>>::type;
    };

    template <std::size_t I, typename T, std::size_t N>
    struct reflected_element<I, std::array<T, N>> {
        static constexpr const char* name = "";
        static constexpr const char* key = "";
        using type = T;
    };

    template <typename T>
    struct reflected {
        static constexpr bool enabled = false;
    };

    namespace detail
    {
        template <typename T, typename Unused = void>
        struct to_variant;
        template <typename... T>
        struct to_variant<std::tuple<T...>> {
            typedef cxutil::variant<typename std::remove_const<T>::type...> type;
        };

        template <typename T>
        struct to_variant<reflected<T>> {
            using type = typename to_variant<typename reflected<T>::tuple_type>::type;
        };

        template <typename T>
        struct to_variant<T, typename std::enable_if<reflected<T>::enabled, void>::type> {
            using type = typename to_variant<reflected<T>>::type;
        };
    } // End of namespace detail

    template <typename T>
    using to_variant_t = typename detail::to_variant<T>::type;

    template <std::size_t I, typename T>
    struct reflected_element<I, reflected<T>> : public reflected_element<I, T> {
    };

    template <typename T>
    using reflectable = typename reflected<T>::enabled;

    template <typename T>
    struct reflected_if_reflectable {
        typedef typename std::conditional<reflected<T>::enabled, reflected<T>, T>::type type;
    };

    namespace detail
    {
        template <std::size_t I, std::size_t N, typename T>
        struct reflected_getter {
            typedef to_variant_t<T> E;
            E operator()(const T& t, size_t n) const
            {
                if (I == n) {
                    return E(reflected_element<I, T>::get(t));
                }
                return reflected_getter<I + 1, N, T>()(t, n);
            }
            E operator()(const T& t, const std::string &n) const
            {
                if (n==reflected_element<I, T>::key()) {
                    return E(reflected_element<I, T>::get(t));
                }
                return reflected_getter<I + 1, N, T>()(t, n);
            }
        };
        template <std::size_t N, typename T>
        struct reflected_getter<N, N, T> {
            typedef to_variant_t<T> E;
            template<typename ...U>
            E operator()(U...) const
            {
                throw std::out_of_range("reflected_getter");
            }
        };
        template <std::size_t I, std::size_t N, typename T>
        struct reflected_setter {
            typedef to_variant_t<T> E;
            void operator()(T& t, const E& e, size_t n) const
            {
                if (I == n) {
                    reflected_element<I, T>::get(t)
                        = e.template get<typename reflected_element<I, T>::type>();
                    return;
                }
                reflected_setter<I + 1, N, T>()(t, e, n);
            }
            void operator()(T& t, E&& e, size_t n) const
            {
                if (I == n) {
                    reflected_element<I, T>::get(t)
                        = e.template get<typename reflected_element<I, T>::type>();
                    return;
                }
                reflected_setter<I + 1, N, T>()(t, std::forward<E>(e), n);
            }
            void operator()(T& t, const E& e, const std::string &n) const
            {
                if (n==reflected_element<I, T>::key()) {
                    reflected_element<I, T>::get(t)
                        = e.template get<typename reflected_element<I, T>::type>();
                    return;
                }
                reflected_setter<I + 1, N, T>()(t, e, n);
            }
            void operator()(T& t, E&& e, const std::string &n) const
            {
                if (n==reflected_element<I, T>::key()) {
                    reflected_element<I, T>::get(t)
                        = e.template get<typename reflected_element<I, T>::type>();
                    return;
                }
                reflected_setter<I + 1, N, T>()(t, std::forward<E>(e), n);
            }
        };
        template <std::size_t N, typename T>
        struct reflected_setter<N, N, T> {
            typedef to_variant_t<T> E;
            template<typename ...U>
            E operator()(U...) const
            {
                throw std::out_of_range("reflected_setter");
            }
        };
    }

    template <typename T>
    typename std::enable_if<reflected<T>::enabled, to_variant_t<T>>::type get(size_t n, const T& t)
    {
        return detail::reflected_getter<0, std::tuple_size<T>::value, T>()(t, n);
    }

    template <typename T>
    typename std::enable_if<reflected<T>::enabled, void>::type
    set(size_t n, T& t, const to_variant_t<T>& e)
    {
        detail::reflected_setter<0, std::tuple_size<T>::value, T>()(t, e, n);
    }

    template <typename T>
    typename std::enable_if<reflected<T>::enabled, void>::type
    set(size_t n, T& t, to_variant_t<T>&& e)
    {
        detail::reflected_setter<0, std::tuple_size<T>::value, T>()(t, std::move(e), n);
    }

    template <typename T>
    typename std::enable_if<reflected<T>::enabled, to_variant_t<T>>::type get(const std::string &n, const T& t)
    {
        return detail::reflected_getter<0, std::tuple_size<T>::value, T>()(t, n);
    }

    template <typename T>
    typename std::enable_if<reflected<T>::enabled, void>::type
    set(const std::string &n, T& t, const to_variant_t<T>& e)
    {
        detail::reflected_setter<0, std::tuple_size<T>::value, T>()(t, e, n);
    }

    template <typename T>
    typename std::enable_if<reflected<T>::enabled, void>::type
    set(const std::string &n, T& t, to_variant_t<T>&& e)
    {
        detail::reflected_setter<0, std::tuple_size<T>::value, T>()(t, std::move(e), n);
    }
}
} // End of namespace cxutil::reflection

namespace std
{
template <std::size_t I, typename T>
typename std::enable_if<cxutil::reflection::reflected<T>::enabled,
                        typename cxutil::reflection::reflected_element<I, T>::type>::type&
get(T& t)
{
    return cxutil::reflection::reflected_element<I, T>::get(t);
}
template <std::size_t I, typename T>
typename std::enable_if<cxutil::reflection::reflected<T>::enabled,
                        typename cxutil::reflection::reflected_element<I, T>::type>::type&&
get(T&& t)
{
    return cxutil::reflection::reflected_element<I, T>::get(t);
}
template <std::size_t I, typename T>
typename std::enable_if<cxutil::reflection::reflected<T>::enabled,
                        typename cxutil::reflection::reflected_element<I, T>::type>::type const&
get(T const& t)
{
    return cxutil::reflection::reflected_element<I, T>::get(t);
}
}

#endif // !defined(CXUTIL_REFLECTION_REFLECTION_IMPL_HPP)
