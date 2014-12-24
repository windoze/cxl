#pragma once

#include <stdexcept>
#include <memory>
#include <utility>
#include <type_traits>
#include <typeinfo>
#include <algorithm>
#include <functional>
#include <cstdlib>
#include <cxutil/type_traits.hpp>
#include <cxutil/variant/recursive_wrapper.hpp>

namespace utility
{

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
struct bad_get : std::runtime_error
{
    explicit bad_get(const char* what) : std::runtime_error(what) { ; }
    explicit bad_get(std::string const& what) : std::runtime_error(what) { ; }
};
#pragma clang diagnostic pop

template <typename... Types>
struct variant final
{
    using size_type = std::size_t;

    static_assert(and_<!std::is_reference<Types>::value...>::value, "xx");
    static_assert(and_<!std::is_const<Types>::value...>::value, "xx");
    static_assert(and_<!std::is_volatile<Types>::value...>::value, "xx");

    static constexpr size_type npos = get_offset<0>::value;

    template <typename T>
    using which_type = get_offset<0, std::is_same<T, unwrap_type<Types>>::value...>;

    template <size_type N>
    using type = nth_type<N, unwrap_type<Types>...>;

    using types_count = uint_t<sizeof...(Types)>;

    template <typename T>
    using is_this_type = bool_t<(which_type<unrefcv<T>>::value != npos)>;

    template <typename... Args>
    using which_is_constructible
        = get_offset<0, std::is_constructible<unwrap_type<Types>, Args...>::value...>;

    template <typename T>
    using which_is_assignable = get_offset<0, std::is_assignable<unwrap_type<Types>, T>::value...>;

    template <typename... Args>
    using is_there_constructible
        = or_<std::is_constructible<unwrap_type<Types>, Args...>::value...>;

    template <typename T>
    using is_there_assignable = or_<std::is_assignable<unwrap_type<Types>, T>::value...>;

    using reference = variant<std::reference_wrapper<unwrap_type<Types>>...>;

private:
    template <size_type N>
    using internal_type = nth_type<N, Types...>;

    using storage_type = std::aligned_union_t<0, Types...>;
    std::unique_ptr<storage_type> storage_ = std::make_unique<storage_type>();
    size_type which_ = npos;

    template <typename ResultType, typename Storage, typename Visitor, typename T, typename... Args>
    static ResultType caller(Storage&& storage, Visitor&& visitor, Args&&... args)
    {
        return std::forward<Visitor>(visitor)(unwrap(reinterpret_cast<T&&>(storage)),
                                              std::forward<Args>(args)...);
    }

    struct destroyer
    {

        template <typename T>
        void operator()(T& value) const
        {
            value.~T();
        }
    };

    template <typename R>
    enable_if<is_this_type<unrefcv<R>>::value> construct(R&& rhs)
    {
        static_assert(std::is_constructible<unrefcv<R>, R>::value,
                      "type selected, but it cannot be constructed");
        constexpr size_type which = which_type<unrefcv<R>>::value;
        ::new (storage_.get()) internal_type<which>(std::forward<R>(rhs));
        which_ = which;
    }

    template <typename... Args>
    void construct(Args&&... args)
    {
        constexpr size_type which = which_is_constructible < Args && ... > ::value;
        static_assert((which != npos),
                      "no one type can be constructed from specified parameter pack");
        // -Wconversion warning here means, that construction or assignment may imply undesirable
        // type conversion
        ::new (storage_.get()) internal_type<which>(std::forward<Args>(args)...);
        which_ = which;
    }

    struct constructor
    {

        template <typename R>
        void operator()(R&& rhs) const
        {
            destination_.construct(std::forward<R>(rhs));
        }

        variant& destination_;
    };

    struct assigner
    {

        template <typename L, typename R>
        enable_if<is_constructible<L, R>> reconstruct(R&& rhs) const noexcept
        {
            variant backup_(std::forward<R>(rhs));
            lhs_.swap(backup_);
        }

        template <typename L, typename R>
        enable_if<!is_constructible<L, R>> reconstruct(R&&) const
        {
            throw bad_get("assigner: lhs is not constructible from rhs");
        }

        template <typename L, typename R>
        enable_if<is_assignable<L, R>> reassign(R&& rhs) const
        {
            // -Wconversion warning here means, that assignment may imply undesirable type
            // conversion
            lhs_.get<L>() = std::forward<R>(rhs);
        }

        template <typename L, typename R>
        enable_if<!is_assignable<L, R>> reassign(R&& rhs) const
        {
            reconstruct<L>(std::forward<R>(rhs));
        }

        template <typename R>
        enable_if<is_this_type<unrefcv<R>>::value> operator()(R&& rhs) const
        {
            using lhs = unrefcv<R>;
            static_assert((is_assignable<lhs, R> || is_constructible<lhs, R>),
                          "type selected, but it cannot be assigned");
            if (lhs_.which() == which_type<lhs>::value) {
                reassign<lhs>(std::forward<R>(rhs));
            } else {
                reconstruct<lhs>(std::forward<R>(rhs));
            }
        }

        template <typename R>
        enable_if<is_there_constructible<R&&>::value> construct(R&& rhs) const noexcept
        {
            variant backup_(std::forward<R>(rhs));
            lhs_.swap(backup_);
        }

        template <typename R>
        enable_if<!is_there_constructible<R&&>::value> construct(R&&) const
        {
            throw bad_get("assigner: there is no constructible from rhs at all");
        }

        template <typename R>
        enable_if<(!is_this_type<unrefcv<R>>::value && is_there_assignable<R&&>::value)>
        operator()(R&& rhs) const
        {
            constexpr size_type which = which_is_assignable<R&&>::value;
            if (lhs_.which() == which) {
                // -Wconversion warning here means, that assignment may imply undesirable type
                // conversion
                lhs_.get<type<which>>() = std::forward<R>(rhs);
            } else {
                construct(std::forward<R>(rhs));
            }
        }

        template <typename R>
        enable_if<(!is_this_type<unrefcv<R>>::value && !is_there_assignable<R&&>::value)>
        operator()(R&& rhs) const
        {
            construct(std::forward<R>(rhs));
        }

        variant& lhs_;
    };

    struct reflect
    {

        template <typename T>
        std::type_info const& operator()(T const&) const
        {
            return typeid(T);
        }
    };

public:
    ~variant() noexcept(and_<std::is_nothrow_destructible<Types>::value...>::value)
    {
        apply_visitor(destroyer{});
    }

    void swap(variant& other) noexcept
    {
        storage_.swap(other.storage_);
        std::swap(which_, other.which_);
    }

    size_type which() const { return which_; }

    template <typename Visitor, typename... Args>
    decltype(auto) apply_visitor(Visitor&& visitor, Args&&... args) const &
    {
        static_assert(is_same<result_of<Visitor&&, unwrap_type<Types> const&, Args&&...>...>,
                      "non-identical return types in visitor");
        using result_type = result_of<Visitor&&, type<0> const&, Args&&...>;
        using caller_type
            = result_type (*)(storage_type const& storage, Visitor&& visitor, Args&&... args);
        static constexpr caller_type dispatcher_[sizeof...(Types)]
            = {&variant::caller < result_type,
               storage_type const&,
               Visitor &&,
               Types const&,
               Args && ... > ...};
        return dispatcher_[which_](
            *storage_, std::forward<Visitor>(visitor), std::forward<Args>(args)...);
    }

    template <typename Visitor, typename... Args>
    decltype(auto) apply_visitor(Visitor&& visitor, Args&&... args) &
    {
        static_assert(
            detail::is_same<result_of<Visitor&&, unwrap_type<Types>&, Args&&...>...>::value,
            "non-identical return types in visitor");
        using result_type = result_of<Visitor&&, type<0>&, Args&&...>;
        using caller_type
            = result_type (*)(storage_type& storage, Visitor&& visitor, Args&&... args);
        static constexpr caller_type dispatcher_[sizeof...(Types)] = {
            &variant::caller < result_type, storage_type&, Visitor &&, Types&, Args && ... > ...};
        return dispatcher_[which_](
            *storage_, std::forward<Visitor>(visitor), std::forward<Args>(args)...);
    }

    template <typename Visitor, typename... Args>
    decltype(auto) apply_visitor(Visitor&& visitor, Args&&... args) &&
    {
        static_assert(is_same<result_of<Visitor&&, unwrap_type<Types>&&, Args&&...>...>,
                      "non-identical return types in visitor");
        using result_type = result_of<Visitor&&, type<0>&&, Args&&...>;
        using caller_type
            = result_type (*)(storage_type&& storage, Visitor&& visitor, Args&&... args);
        static constexpr caller_type dispatcher_[sizeof...(Types)]
            = {&variant::caller < result_type,
               storage_type &&,
               Visitor &&,
               Types &&,
               Args && ... > ...};
        return dispatcher_[which_](
            std::move(*storage_), std::forward<Visitor>(visitor), std::forward<Args>(args)...);
    }

    variant()
    {
        static_assert(is_there_constructible<>::value, "no one type is default constructible");
        construct();
    }

    variant(variant const& rhs) { rhs.apply_visitor(constructor{*this}); }

    variant(variant& rhs) { rhs.apply_visitor(constructor{*this}); }

    variant(variant&& rhs) noexcept { std::move(rhs).apply_visitor(constructor{*this}); }

    template <typename... OtherTypes>
    variant(variant<OtherTypes...> const& rhs)
    {
        rhs.apply_visitor(constructor{*this});
    }

    template <typename... OtherTypes>
    variant(variant<OtherTypes...>& rhs)
    {
        rhs.apply_visitor(constructor{*this});
    }

    template <typename... OtherTypes>
    variant(variant<OtherTypes...>&& rhs)
    {
        std::move(rhs).apply_visitor(constructor{*this});
    }

    template <typename First, typename... Rest>
    variant(First&& first, Rest&&... rest)
    {
        construct<First&&, Rest&&...>(std::forward<First>(first), std::forward<Rest>(rest)...);
    }

    variant& operator=(variant const& rhs)
    {
        rhs.apply_visitor(assigner{*this});
        return *this;
    }

    variant& operator=(variant& rhs)
    {
        rhs.apply_visitor(assigner{*this});
        return *this;
    }

    variant& operator=(variant&& rhs) noexcept
    {
        std::move(rhs).apply_visitor(assigner{*this});
        return *this;
    }

    template <typename... OtherTypes>
    variant& operator=(variant<OtherTypes...> const& rhs)
    {
        rhs.apply_visitor(assigner{*this});
        return *this;
    }

    template <typename... OtherTypes>
    variant& operator=(variant<OtherTypes...>& rhs)
    {
        rhs.apply_visitor(assigner{*this});
        return *this;
    }

    template <typename... OtherTypes>
    variant& operator=(variant<OtherTypes...>&& rhs)
    {
        std::move(rhs).apply_visitor(assigner{*this});
        return *this;
    }

    template <typename R>
    variant& operator=(R&& rhs)
    {
        static_assert((is_this_type<unrefcv<R>>::value
                       || (is_there_assignable<R&&>::value || is_there_constructible<R&&>::value)),
                      "no one underlying type is proper to assignment");
        assigner{*this}(std::forward<R>(rhs));
        return *this;
    }

    template <typename T>
    T const& get() const &
    {
        constexpr size_type which = which_type<T>::value;
        static_assert((which != npos), "type is not in the list");
        if (which_ != which) {
            throw bad_get("get: containing type does not match requested type");
        } else {
            return unwrap(reinterpret_cast<internal_type<which> const&>(*storage_));
        }
    }

    template <size_type N>
    decltype(auto) get() const &
    {
        if (which_ != N) {
            throw bad_get("get: containing type does not match requested type");
        } else {
            return unwrap(reinterpret_cast<internal_type<N> const&>(*storage_));
        }
    }

    template <typename T>
    T& get() &
    {
        constexpr size_type which = which_type<T>::value;
        static_assert((which != npos), "type is not in the list");
        if (which_ != which) {
            throw bad_get("get: containing type does not match requested type");
        } else {
            return unwrap(reinterpret_cast<internal_type<which>&>(*storage_));
        }
    }

    template <size_type N>
    decltype(auto) get() &
    {
        if (which_ != N) {
            throw bad_get("get: containing type does not match requested type");
        } else {
            return unwrap(reinterpret_cast<internal_type<N>&>(*storage_));
        }
    }

    template <typename T>
    T&& get() &&
    {
        constexpr size_type which = which_type<T>::value;
        static_assert((which != npos), "type is not in the list");
        if (which_ != which) {
            throw bad_get("get: containing type does not match requested type");
        } else {
            return unwrap(reinterpret_cast<internal_type<which>&&>(*storage_));
        }
    }

    template <size_type N>
    decltype(auto) get() &&
    {
        if (which_ != N) {
            throw bad_get("get: containing type does not match requested type");
        } else {
            return unwrap(reinterpret_cast<internal_type<N>&&>(*storage_));
        }
    }

    std::type_info const& get_type_info() const { return apply_visitor(reflect{}); }
};

template <>
struct variant<>; // Intentionally declared but not defined. This leaves open the possibility to
// define a meaningful specialization by yourself.

template <typename VariantType, typename T>
constexpr typename VariantType::size_type which_bounded_type
    = VariantType::template which_type<T>::value;

template <typename T>
struct is_variant : bool_t<false>
{
};

template <typename First, typename... Rest>
struct is_variant<variant<First, Rest...>> : bool_t<true>
{
};

template <typename T>
constexpr bool is_variant_v = is_variant<unrefcv<T>>::value;

template <typename... Types>
void swap(variant<Types...>& lhs, variant<Types...>& rhs) noexcept
{
    lhs.swap(rhs);
}

template <typename Variant, typename... Args>
Variant make_variant(Args&&... args)
{
    return {std::forward<Args>(args)...};
}

template <typename T, typename... Types>
T const& get(variant<Types...> const& variant)
{
    return variant.template get<T>();
}

template <typename T, typename... Types>
T& get(variant<Types...>& variant)
{
    return variant.template get<T>();
}

template <typename T, typename... Types>
T&& get(variant<Types...>&& variant)
{
    return std::move(variant).template get<T>();
}

namespace detail
{
    template <typename T, typename... Types>
    struct contained_t<T, variant<Types...>> : contained_t<T, Types...>
    {
    };
}   // End of namespace utility::detail
}   // End of namespace utility
