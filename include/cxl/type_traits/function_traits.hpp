#ifndef CXL_FUNCTION_TRAITS_HPP
#define CXL_FUNCTION_TRAITS_HPP

#pragma once

#include <functional>
#include <tuple>
#include <type_traits>

namespace cxl {
    /// make_tuple_indices
    template<std::size_t...>
    struct tuple_indices
    {
    };

    namespace detail {
        template<std::size_t Sp, class IntTuple, std::size_t Ep>
        struct make_indices_imp;
        template<std::size_t Sp, std::size_t... Indices, std::size_t Ep>
        struct make_indices_imp<Sp, tuple_indices<Indices...>, Ep>
        {
            typedef typename make_indices_imp<Sp + 1, tuple_indices<Indices..., Sp>, Ep>::type type;
        };

        template<std::size_t Ep, std::size_t... Indices>
        struct make_indices_imp<Ep, tuple_indices<Indices...>, Ep>
        {
            typedef tuple_indices<Indices...> type;
        };
    } // End or namespace cxl::detail

    template<std::size_t Ep, std::size_t Sp = 0>
    struct make_tuple_indices
    {
        static_assert(Sp <= Ep, "make_tuple_indices input error");
        typedef typename detail::make_indices_imp<Sp, tuple_indices<>, Ep>::type type;
    };

    /// invoke
    template<class Fp, class A0, class... Args>
    inline auto invoke(Fp &&f, A0 &&a0, Args &&... args)
    -> decltype((std::forward<A0>(a0).*f)(std::forward<Args>(args)...))
    {
        return (std::forward<A0>(a0).*f)(std::forward<Args>(args)...);
    }

    template<class Fp, class A0, class... Args>
    inline auto invoke(Fp &&f, A0 &&a0, Args &&... args)
    -> decltype(((*std::forward<A0>(a0)).*f)(std::forward<Args>(args)...))
    {
        return ((*std::forward<A0>(a0)).*f)(std::forward<Args>(args)...);
    }

    template<class Fp, class A0>
    inline auto invoke(Fp &&f, A0 &&a0) -> decltype(std::forward<A0>(a0).*f)
    {
        return std::forward<A0>(a0).*f;
    }

    template<class Fp, class A0>
    inline auto invoke(Fp &&f, A0 &&a0) -> decltype((*std::forward<A0>(a0)).*f)
    {
        return (*std::forward<A0>(a0)).*f;
    }

    template<class Fp, class... Args>
    inline auto invoke(Fp &&f, Args &&... args)
    -> decltype(std::forward<Fp>(f)(std::forward<Args>(args)...))
    {
        return std::forward<Fp>(f)(std::forward<Args>(args)...);
    }

    /// get_signature
    namespace detail {
        // remove_class
        template<typename T>
        struct remove_class
        {
        };
        template<typename C, typename R, typename... A>
        struct remove_class<R (C::*)(A...)>
        {
            using type = R(A...);
        };
        template<typename C, typename R, typename... A>
        struct remove_class<R (C::*)(A...) const>
        {
            using type = R(A...);
        };
        template<typename C, typename R, typename... A>
        struct remove_class<R (C::*)(A...) volatile>
        {
            using type = R(A...);
        };
        template<typename C, typename R, typename... A>
        struct remove_class<R (C::*)(A...) const volatile>
        {
            using type = R(A...);
        };

        // get_signature_impl
        template<typename T>
        struct get_signature_impl
        {
            using type
            = typename remove_class<decltype(&std::remove_reference<T>::type::operator())>::type;
        };
        template<typename R, typename... A>
        struct get_signature_impl<R(A...)>
        {
            using type = R(A...);
        };
        template<typename R, typename... A>
        struct get_signature_impl<R (&)(A...)>
        {
            using type = R(A...);
        };
        template<typename R, typename... A>
        struct get_signature_impl<R (*)(A...)>
        {
            using type = R(A...);
        };
    } // End of namespace cxl::detail
    template<typename T>
    using get_signature = typename detail::get_signature_impl<T>::type;

    /// make_function_type
    template<typename F>
    using make_function_type = std::function<get_signature<F>>;

    /// make_function
    template<typename F>
    make_function_type<F> make_function(F &&f)
    {
        return make_function_type<F>(std::forward<F>(f));
    }

    /// Function traits
    template<typename T>
    struct function_traits
    {
        typedef typename detail::remove_class<decltype(
                                              &std::remove_reference<T>::type::operator())>::type call_type;
        enum
        {
            arity = function_traits<call_type>::arity
        };
        using result_type = typename function_traits<call_type>::result_type;
        using arguments_tuple = typename function_traits<call_type>::arguments_tuple;
        template<size_t N>
        using arg = typename function_traits<call_type>::template arg<N>;
    };
    template<typename R, typename... A>
    struct function_traits<R(A...)>
    {
        enum
        {
            arity = sizeof...(A)
        };
        typedef R result_type;
        typedef std::tuple<A...> arguments_tuple;
        template<size_t N>
        struct arg
        {
            typedef typename std::tuple_element<N, arguments_tuple>::type type;
        };
    };
    template<typename R, typename... A>
    struct function_traits<R (&)(A...)>
    {
        enum
        {
            arity = sizeof...(A)
        };
        typedef R result_type;
        typedef std::tuple<A...> arguments_tuple;
        template<size_t N>
        struct arg
        {
            typedef typename std::tuple_element<N, arguments_tuple>::type type;
        };
    };
    template<typename R, typename... A>
    struct function_traits<R (*)(A...)>
    {
        enum
        {
            arity = sizeof...(A)
        };
        typedef R result_type;
        typedef std::tuple<A...> arguments_tuple;
        template<size_t N>
        struct arg
        {
            typedef typename std::tuple_element<N, arguments_tuple>::type type;
        };
    };
    template<typename R, typename C, typename... A>
    struct function_traits<R (C::*)(A...)>
    {
        enum
        {
            arity = sizeof...(A)
        };
        typedef R result_type;
        typedef std::tuple<A...> arguments_tuple;
        template<size_t N>
        struct arg
        {
            typedef typename std::tuple_element<N, arguments_tuple>::type type;
        };
    };
    template<typename R, typename C, typename... A>
    struct function_traits<R (C::*)(A...) const>
    {
        enum
        {
            arity = sizeof...(A)
        };
        typedef R result_type;
        typedef std::tuple<A...> arguments_tuple;
        template<size_t N>
        struct arg
        {
            typedef typename std::tuple_element<N, arguments_tuple>::type type;
        };
    };
    template<typename R, typename C, typename... A>
    struct function_traits<R (C::*)(A...) volatile>
    {
        enum
        {
            arity = sizeof...(A)
        };
        typedef R result_type;
        typedef std::tuple<A...> arguments_tuple;
        template<size_t N>
        struct arg
        {
            typedef typename std::tuple_element<N, arguments_tuple>::type type;
        };
    };
    template<typename R, typename C, typename... A>
    struct function_traits<R (C::*)(A...) const volatile>
    {
        enum
        {
            arity = sizeof...(A)
        };
        typedef R result_type;
        typedef std::tuple<A...> arguments_tuple;
        template<size_t N>
        struct arg
        {
            typedef typename std::tuple_element<N, arguments_tuple>::type type;
        };
    };
} // End of namespace cxl
#endif // CXL_FUNCTION_TRAITS_HPP
