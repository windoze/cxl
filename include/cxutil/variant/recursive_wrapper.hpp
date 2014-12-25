#ifndef CXUTIL_VARIANT_RECURSIVE_WRAPPER_HPP
#define CXUTIL_VARIANT_RECURSIVE_WRAPPER_HPP

#pragma once

#include <memory>
#include <utility>
#include <cxutil/type_traits.hpp>

namespace cxutil
{

template <typename WrappedType>
struct recursive_wrapper final
{
    static_assert(!std::is_reference<WrappedType>::value, "xx");
    static_assert(!std::is_const<WrappedType>::value, "xx");
    static_assert(!std::is_volatile<WrappedType>::value, "xx");

    using type = WrappedType;

    ~recursive_wrapper() noexcept = default;

    template <typename First, typename... Rest>
    recursive_wrapper(First&& first, Rest&&... rest)
    : storage_(std::make_unique<type>(std::forward<First>(first), std::forward<Rest>(rest)...))
    {
    }

    recursive_wrapper() : storage_(std::make_unique<type>()) { ; }

    recursive_wrapper(recursive_wrapper const& rhs) : recursive_wrapper(rhs.get()) { ; }

    recursive_wrapper(recursive_wrapper& rhs) : recursive_wrapper(rhs.get()) { ; }

    recursive_wrapper(recursive_wrapper&& rhs) noexcept : recursive_wrapper(std::move(rhs).get())
    {
    }

    recursive_wrapper& operator=(recursive_wrapper const& rhs) { get() = rhs.get(); }

    recursive_wrapper& operator=(recursive_wrapper& rhs) { get() = rhs.get(); }

    recursive_wrapper& operator=(recursive_wrapper&& rhs) noexcept { get() = std::move(rhs).get(); }

    recursive_wrapper& operator=(type const& rhs)
    {
        get() = rhs;
        return *this;
    }

    recursive_wrapper& operator=(type& rhs)
    {
        get() = rhs;
        return *this;
    }

    recursive_wrapper& operator=(type&& rhs) noexcept
    {
        get() = std::move(rhs);
        return *this;
    }

    void swap(recursive_wrapper& rhs) noexcept { std::swap(storage_, rhs.storage_); }

    type const& get() const & { return *storage_; }

    type& get() & { return *storage_; }

    type&& get() && { return std::move(*storage_); }

    explicit operator type const&() const & { return get(); }

    explicit operator type&() & { return get(); }

    explicit operator type&&() && { return std::move(get()); }

private:
    std::unique_ptr<type> storage_;
};

template <typename T>
struct is_recursive_wrapper : bool_t<false>
{
};

template <typename WrappedType>
struct is_recursive_wrapper<recursive_wrapper<WrappedType>> : bool_t<true>
{
};

namespace detail
{

    template <typename Nonwrapped, typename Model>
    struct unwrap_type
    {
        using type = Model;
        Model operator()(Model value) const noexcept { return static_cast<Model>(value); }
    };

    template <typename Wrapped, typename Model>
    struct unwrap_type<recursive_wrapper<Wrapped>, Model>
    {
        using type = copy_refcv<Model, Wrapped>;
        type operator()(Model value) const noexcept
        {
            return static_cast<type>(std::forward<Model>(value).get());
        }
    };
}   // End of namespace cxutil::detail

template <typename Wrapped>
using unwrap_type = typename detail::unwrap_type<unrefcv<Wrapped>, Wrapped>::type;

template <typename T>
decltype(auto) unwrap(T&& value) noexcept
{
    detail::unwrap_type<unrefcv<T>, T&&> unwrap_type;
    return unwrap_type(std::forward<T>(value));
}

template <typename WrappedType>
void swap(recursive_wrapper<WrappedType>& lhs, recursive_wrapper<WrappedType>& rhs) noexcept
{
    lhs.swap(rhs);
}
}   // End of namespace cxutil

#endif  // CXUTIL_VARIANT_RECURSIVE_WRAPPER_HPP
