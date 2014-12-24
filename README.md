cxxstuff
========

Some C++ utilities

* `function_traits` include `arity`, `result_type` and `arg<N>`, also defined `arguments_tuple` that contains all argument types
* `make_function` creates std::function wrapper that matches the original function signature
* `make_function_type<F>` is the correspoding type for std::function wrapper
* `variant` Similiar to Boost.Variant, with full `move` and rvalue-reference support, less hacks, i.e. avoid using of `boost::detail::variant::void_` to fulfill all template arguments.
