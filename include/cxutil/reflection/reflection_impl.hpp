#ifndef CXUTIL_REFLECTION_REFLECTION_IMPL_HPP
#define CXUTIL_REFLECTION_REFLECTION_IMPL_HPP

#include <string>
#include <utility>
#include <tuple>
#include <cxutil/variant.hpp>

namespace cxutil
{
namespace reflection
{
    namespace metadata_store
    {
        // All macro generated cxl_reflected_metadata type will be put in this namespace
        template <typename T>
        struct cxl_reflected_metadata {
            static constexpr bool enabled = false;
        };

        // Predefined metadata for tuple, pair, and array
        template <typename... T>
        struct cxl_reflected_metadata<std::tuple<T...>> {
            static constexpr bool enabled = true;
            typedef std::tuple<T...> owner_type;

            static constexpr char const* name() { return ""; }

            static constexpr unsigned long element_count = std::tuple_size<owner_type>::value;

            template <size_t I, typename Unused>
            struct element {
                static constexpr bool use_value = false;
                static constexpr const unsigned long index = I;
                using type = typename std::tuple_element<I, owner_type>::type;

                static constexpr char const* name() { return ""; }

                static constexpr char const* key() { return ""; }

                template <typename Owner>
                static constexpr std::enable_if_t<!std::is_const<type>::value, type>& get(Owner& d)
                {
                    return std::get<I>(d);
                }

                template <typename Owner>
                static constexpr std::enable_if_t<!std::is_const<type>::value, type>&&
                get(Owner&& d)
                {
                    return std::get<I>(std::move(d));
                }

                template <typename Owner, typename V>
                static std::enable_if_t<!std::is_const<type>::value> set(Owner& d, V&& v)
                {
                    std::get<I>(d) = std::forward<V>(v);
                }

                static constexpr type const& get(owner_type const& d) { return std::get<I>(d); }
            };
        };

        template <typename T, typename U>
        struct cxl_reflected_metadata<std::pair<T, U>> {
            static constexpr bool enabled = true;
            typedef std::pair<T, U> owner_type;

            static constexpr char const* name() { return ""; }

            static constexpr unsigned long element_count = 2;

            template <size_t I, typename Unused>
            struct element;

            template <typename Unused>
            struct element<0, Unused> {
                static constexpr bool use_value = false;
                static constexpr const unsigned long index = 0;
                using type = T;

                static constexpr char const* name() { return "first"; }

                static constexpr char const* key() { return "first"; }

                template <typename Owner>
                static constexpr std::enable_if_t<!std::is_const<typename Owner::first_type>::value,
                                                  type>&
                get(Owner& d)
                {
                    return d.first;
                }

                template <typename Owner>
                static constexpr std::enable_if_t<!std::is_const<typename Owner::first_type>::value,
                                                  type>&&
                get(Owner&& d)
                {
                    return std::move(d.first);
                }

                template <typename Owner, typename V>
                static std::enable_if_t<!std::is_const<typename Owner::first_type>::value>
                set(Owner& d, V&& v)
                {
                    d.first = std::forward<V>(v);
                }

                static constexpr type const& get(owner_type const& d) { return d.first; }
            };

            template <typename Unused>
            struct element<1, Unused> {
                static constexpr bool use_value = false;
                static constexpr const unsigned long index = 1;
                using type = U;

                static constexpr char const* name() { return "second"; }

                static constexpr char const* key() { return "second"; }

                template <typename Owner>
                static constexpr std::
                    enable_if_t<!std::is_const<typename Owner::second_type>::value, type>&
                    get(Owner& d)
                {
                    return d.second;
                }

                template <typename Owner>
                static constexpr std::
                    enable_if_t<!std::is_const<typename Owner::second_type>::value, type>&&
                    get(Owner&& d)
                {
                    return std::move(d.second);
                }

                template <typename Owner, typename V>
                static std::enable_if_t<!std::is_const<typename Owner::second_type>::value>
                set(Owner& d, V&& v)
                {
                    d.second = std::forward<V>(v);
                }

                static constexpr type const& get(owner_type const& d) { return d.second; }
            };
        };

        template <typename T, std::size_t N>
        struct cxl_reflected_metadata<std::array<T, N>> {
            static constexpr bool enabled = true;
            typedef std::array<T, N> owner_type;

            static constexpr char const* name() { return ""; }

            static constexpr unsigned long element_count = N;

            template <size_t I, typename Unused>
            struct element {
                static constexpr bool use_value = false;
                static constexpr const unsigned long index = I;
                using type = T;

                static constexpr char const* name() { return ""; }

                static constexpr char const* key() { return ""; }

                template <typename Owner>
                static constexpr std::enable_if_t<!std::is_const<typename Owner::value_type>::value,
                                                  type>&
                get(owner_type& d)
                {
                    return std::get<I>(d);
                }

                template <typename Owner>
                static constexpr std::enable_if_t<!std::is_const<typename Owner::value_type>::value,
                                                  type>&&
                get(owner_type&& d)
                {
                    return std::get<I>(std::move(d));
                }

                template <typename Owner, typename V>
                static std::enable_if_t<!std::is_const<type>::value> set(Owner& d, V&& v)
                {
                    std::get<I>(d) = std::forward<V>(v);
                }
                static constexpr type const& get(owner_type const& d) { return std::get<I>(d); }
            };
        };
    };

    template <typename T, typename = void>
    struct metadata {
    };

    template <typename T>
    struct metadata<T, std::enable_if_t<T::cxl_reflected_metadata::enabled>> {
        using type = typename T::cxl_reflected_metadata;
    };

    template <typename T>
    struct metadata<T, std::enable_if_t<metadata_store::cxl_reflected_metadata<T>::enabled>> {
        using type = typename metadata_store::cxl_reflected_metadata<T>;
    };

    template <typename T>
    using metadata_t = typename metadata<T>::type;

    template <std::size_t I, typename T, typename = void>
    struct reflected_element {
    };

    template <std::size_t I, typename T>
    using reflected_element_type = typename reflected_element<I, T>::type;

    template <typename T, typename = void>
    struct reflected {
        static constexpr bool enabled = false;
    };

    template <typename T>
    constexpr bool reflectable = reflected<T>::enabled;

    // SFINAE friendly tuple_size
    template <typename T, typename = void>
    struct tuple_size {
    };

    template <class T>
    class tuple_size<const T> : public tuple_size<T>
    {
    };

    template <class T>
    class tuple_size<volatile T> : public tuple_size<T>
    {
    };

    template <class T>
    class tuple_size<const volatile T> : public tuple_size<T>
    {
    };

    // SFINAE friendly tuple_element
    template <std::size_t I, typename T, typename = void>
    class tuple_element
    {
    };

    namespace detail
    {
        template <typename T>
        struct const_tuple {
        };

        template <typename... T>
        struct const_tuple<std::tuple<T...>> {
            using type = std::tuple<typename std::add_const<T>::type...>;
        };

        template <typename T, typename Unused = void>
        struct to_variant;

        template <typename... T>
        struct to_variant<std::tuple<T...>> {
            // Need to dedup as variant with more than one same subtype may cause problem
            typedef deduped<cxutil::variant<unrefcv<T>...>> type;
        };

        template <typename T>
        struct is_tuple {
            static constexpr bool value = false;
        };

        template <typename... T>
        struct is_tuple<std::tuple<T...>> {
            static constexpr bool value = true;
        };

        template <typename T>
        struct to_variant<T, std::enable_if_t<reflectable<T> && !is_tuple<T>::value>> {
            using type = typename to_variant<typename reflected<T>::tuple_type>::type;
        };

        template <typename T, typename Seq>
        struct get_tuple_type_impl;

        template <typename T, std::size_t... I>
        struct get_tuple_type_impl<T, std::index_sequence<I...>> {
            using type = std::tuple<reflected_element_type<I, T>...>;
        };

        template <typename T>
        struct get_tuple_type {
            static constexpr std::size_t count = metadata_t<T>::element_count;
            using type = typename get_tuple_type_impl<T, std::make_index_sequence<count>>::type;
        };

        template <typename... T>
        struct get_tuple_type<std::tuple<T...>> {
            using type = std::tuple<T...>;
        };
    } // End of namespace detail

    template <typename T>
    using to_variant_t = typename detail::to_variant<T>::type;

    template <std::size_t I, typename T>
    struct reflected_element<I, T, std::enable_if_t<reflectable<T> && !std::is_const<T>::value>>
        : public metadata_t<T>::template element<I, void> {
    };

    template <std::size_t I, typename T>
    struct reflected_element<I, const T> : public reflected_element<I, T> {
        using type = std::add_const_t<reflected_element_type<I, T>>;
    };

    template <typename T>
    struct reflected<T, std::enable_if_t<metadata_t<T>::enabled>> : public metadata_t<T> {
        typedef metadata_t<T> impl_type;
        using impl_type::element_count;
        using tuple_type = typename detail::get_tuple_type<T>::type;
        typedef to_variant_t<tuple_type> variant_type;

        static constexpr std::size_t size() { return impl_type::element_count; }
    };

    template <typename T>
    struct reflected<const T> : public reflected<T> {
        using tuple_type = typename detail::const_tuple<typename reflected<T>::tuple_type>::type;
    };

    template <typename T>
    class tuple_size<T, std::enable_if_t<reflectable<T>>>
        : public std::tuple_size<typename reflected<T>::tuple_type>
    {
    };

    template <std::size_t I, typename T>
    class tuple_element<I, T, std::enable_if_t<!reflectable<T>>> : public std::tuple_element<I, T>
    {
    };

    template <std::size_t I, typename T>
    class tuple_element<I, T, std::enable_if_t<reflectable<T> && !std::is_const<T>::value>>
    {
    public:
        using type = reflected_element_type<I, T>;
    };

    template <std::size_t I, typename T>
    class tuple_element<I, const T>
    {
    public:
        using type = std::add_const_t<typename tuple_element<I, T>::type>;
    };

    template <std::size_t I, typename T>
    class tuple_element<I, volatile T>
    {
    public:
        using type = std::add_volatile_t<typename tuple_element<I, T>::type>;
    };

    template <std::size_t I, typename T>
    class tuple_element<I, const volatile T>
    {
    public:
        using type = std::add_cv_t<typename tuple_element<I, T>::type>;
    };

    namespace detail
    {
        template <std::size_t I, std::size_t N, typename T>
        struct reflected_getter {
            typedef to_variant_t<T> Variant;

            Variant operator()(const T& t, size_t n) const
            {
                if (I == n) {
                    return Variant(reflected_element<I, T>::get(t));
                }
                return reflected_getter<I + 1, N, T>()(t, n);
            }

            Variant operator()(const T& t, const std::string& n) const
            {
                if (n == reflected_element<I, T>::key()) {
                    return Variant(reflected_element<I, T>::get(t));
                }
                return reflected_getter<I + 1, N, T>()(t, n);
            }
        };

        template <std::size_t N, typename T>
        struct reflected_getter<N, N, T> {
            template <typename... U>
            to_variant_t<T> operator()(U...) const
            {
                throw std::out_of_range("reflected_getter");
            }
        };
        template <std::size_t I, std::size_t N, typename T>
        struct reflected_setter {
            typedef to_variant_t<T> Variant;

            template <typename U>
            std::enable_if_t<!std::is_const<reflected_element_type<I, U>>::value>
            operator()(U& t, const Variant& e, size_t n) const
            {
                if (I == n) {
                    reflected_element<I, U>::set(t, e.template get<reflected_element_type<I, U>>());
                    return;
                }
                reflected_setter<I + 1, N, U>()(t, e, n);
            }

            template <typename U>
            std::enable_if_t<!std::is_const<reflected_element_type<I, U>>::value>
            operator()(U& t, Variant&& e, size_t n) const
            {
                if (I == n) {
                    reflected_element<I, U>::set(t, e.template get<reflected_element_type<I, U>>());
                    return;
                }
                reflected_setter<I + 1, N, U>()(t, std::forward<Variant>(e), n);
            }

            template <typename U>
            std::enable_if_t<!std::is_const<reflected_element_type<I, U>>::value>
            operator()(U& t, const Variant& e, const std::string& n) const
            {
                if (n == reflected_element<I, U>::key()) {
                    reflected_element<I, U>::set(t, e.template get<reflected_element_type<I, U>>());
                    return;
                }
                reflected_setter<I + 1, N, U>()(t, e, n);
            }

            template <typename U>
            std::enable_if_t<!std::is_const<reflected_element_type<I, U>>::value>
            operator()(U& t, Variant&& e, const std::string& n) const
            {
                if (n == reflected_element<I, U>::key()) {
                    reflected_element<I, U>::set(t, e.template get<reflected_element_type<I, U>>());
                    return;
                }
                reflected_setter<I + 1, N, U>()(t, std::forward<Variant>(e), n);
            }

            template <typename U, typename V>
            std::enable_if_t<std::is_const<reflected_element_type<I, U>>::value>
            operator()(U& t, V&& e, size_t n) const
            {
                if (I == n) {
                    throw std::bad_cast();
                }
                reflected_setter<I + 1, N, U>()(t, std::forward<Variant>(e), n);
            }

            template <typename U, typename V>
            std::enable_if_t<std::is_const<reflected_element_type<I, U>>::value>
            operator()(U& t, V&& e, const std::string& n) const
            {
                if (n == reflected_element<I, U>::key()) {
                    throw std::bad_cast();
                }
                reflected_setter<I + 1, N, U>()(t, std::forward<Variant>(e), n);
            }
        };

        template <std::size_t N, typename T>
        struct reflected_setter<N, N, T> {
            template <typename... U>
            void operator()(U...) const
            {
                throw std::out_of_range("reflected_setter");
            }
        };

        template <std::size_t I, std::size_t N, typename T>
        struct member_name_visitor {
            template <typename F>
            void operator()(const T& t, F&& f) const
            {
                f(t, I, reflected_element<I, T>::name());
                member_name_visitor<I + 1, N, T>()(t, std::forward<F>(f));
            }
        };

        template <std::size_t N, typename T>
        struct member_name_visitor<N, N, T> {
            template <typename F>
            void operator()(const T& t, F&& f) const
            {
            }
        };

        template <std::size_t I, std::size_t N, typename T>
        struct member_key_visitor {
            template <typename F>
            void operator()(const T& t, F&& f) const
            {
                f(t, I, reflected_element<I, T>::key());
                member_key_visitor<I + 1, N, T>()(t, std::forward<F>(f));
            }
        };

        template <std::size_t N, typename T>
        struct member_key_visitor<N, N, T> {
            template <typename F>
            void operator()(const T& t, F&& f) const
            {
            }
        };
    }

    template <typename T>
    std::enable_if_t<reflectable<T>, to_variant_t<T>> get_variant(size_t n, const T& t)
    {
        return detail::reflected_getter<0, tuple_size<T>::value, T>()(t, n);
    }

    template <typename T>
    std::enable_if_t<reflectable<T>, to_variant_t<T>> get_variant(const std::string& n, const T& t)
    {
        return detail::reflected_getter<0, tuple_size<T>::value, T>()(t, n);
    }

    template <typename U, typename T>
    std::enable_if_t<reflectable<T>, U> get(size_t n, const T& t)
    {
        return get_variant(n, t).template get<U>();
    }

    template <typename U, typename T>
    std::enable_if_t<reflectable<T>, U> get(const std::string& n, const T& t)
    {
        return get_variant(n, t).template get<U>();
    }

    template <typename T>
    std::enable_if_t<reflectable<T>, void> set(size_t n, T& t, const to_variant_t<T>& e)
    {
        detail::reflected_setter<0, tuple_size<T>::value, T>()(t, e, n);
    }

    template <typename T>
    std::enable_if_t<reflectable<T>, void> set(size_t n, T& t, to_variant_t<T>&& e)
    {
        detail::reflected_setter<0, tuple_size<T>::value, T>()(t, std::move(e), n);
    }

    template <typename T>
    std::enable_if_t<reflectable<T>, void> set(const std::string& n, T& t, const to_variant_t<T>& e)
    {
        detail::reflected_setter<0, tuple_size<T>::value, T>()(t, e, n);
    }

    template <typename T>
    std::enable_if_t<reflectable<T>, void> set(const std::string& n, T& t, to_variant_t<T>&& e)
    {
        detail::reflected_setter<0, tuple_size<T>::value, T>()(t, std::move(e), n);
    }

    template <typename T, typename F>
    std::enable_if_t<reflectable<T>, void> for_each_member_key(const T& t, F&& f)
    {
        detail::member_key_visitor<0, tuple_size<T>::value, T>()(t, std::forward<F>(f));
    }

    template <typename T, typename F>
    std::enable_if_t<reflectable<T>, void> for_each_member_name(const T& t, F&& f)
    {
        detail::member_name_visitor<0, tuple_size<T>::value, T>()(t, std::forward<F>(f));
    }
} // End of namespace cxutil::reflection
using reflection::reflectable;
using reflection::to_variant_t;
using reflection::tuple_size;
using reflection::tuple_element;
using reflection::get_variant;
using reflection::get;
using reflection::set;
using reflection::for_each_member_name;
using reflection::for_each_member_key;
} // End of namespace cxutil

namespace std
{
template <std::size_t I, typename T>
std::enable_if_t<cxutil::reflectable<T> && !cxutil::reflection::reflected_element<I, T>::use_value,
                 cxutil::reflection::reflected_element_type<I, T>>&
get(T& t)
{
    return cxutil::reflection::reflected_element<I, T>::get(t);
}

template <std::size_t I, typename T>
std::enable_if_t<cxutil::reflectable<T> && !cxutil::reflection::reflected_element<I, T>::use_value,
                 cxutil::reflection::reflected_element_type<I, T>>&&
get(T&& t)
{
    return cxutil::reflection::reflected_element<I, T>::get(t);
}

template <std::size_t I, typename T>
std::enable_if_t<cxutil::reflectable<T> && !cxutil::reflection::reflected_element<I, T>::use_value,
                 cxutil::reflection::reflected_element_type<I, T>> const&
get(T const& t)
{
    return cxutil::reflection::reflected_element<I, T>::get(t);
}

// Get attribute via a getter
template <std::size_t I, typename T>
std::enable_if_t<cxutil::reflectable<T> && cxutil::reflection::reflected_element<I, T>::use_value,
                 cxutil::reflection::reflected_element_type<I, T>>
get(T const& t)
{
    return cxutil::reflection::reflected_element<I, T>::get(t);
}
}

#endif // !defined(CXUTIL_REFLECTION_REFLECTION_IMPL_HPP)
