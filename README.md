cxxstuff
========

Some C++ utilities

* `function_traits` include `arity`, `result_type` and `arg<N>`, also defined `arguments_tuple` that contains all argument types
* `make_function` creates std::function wrapper that matches the original function signature
* `make_function_type<F>` is the correspoding type for std::function wrapper
* `variant` Similiar to Boost.Variant, with full `move` and rvalue-reference support, less hacks, i.e. avoid using of `boost::detail::variant::void_` to fulfill all template arguments.
* Minimal reflection support by defining some metadata with macros:
    - `CXL_BEGIN_REFLECTED(TYPE, COUNT, ...)` begins a embedded metadata definition:
        + `TYPE` the type that metadata is embedded in.
        + `COUNT` number of elements belong to `TYPE`.
        + Also some additional attributes can be defined:
            * `CXL_SQL_TABLE(X)` attach a `sql_table()` attribute, `X` is a string literal.
            * `CXL_XML_NODE(X)` attach a `xml_node()` attribute, `X` is a string literal.
            * `CXL_XML_NAMESPACE` attach a `xml_namespace()` attribute, `X` is a string literal.
    - `CXL_END_REFLECTED()` ends a embedded metadata definition
    - `CXL_EXT_BEGIN_REFLECTED(TYPE, COUNT, ...)` begins a standalone metadata definition, this macro must appear in the global namespace, besides this, it has same syntax and effects as `CXL_BEGIN_REFLECTED(TYPE, COUNT, ...)`.
    - `CXL_EXT_END_REFLECTED()` same as `CXL_END_REFLECTED()`, but ends a standalone metadata definition.
    - `CXL_REFLECTED_MEMBER(INDEX, NAME, ...)` define the metadata of a member, must appear inside `CXL_BEGIN_REFLECTED`/`CXL_EXT_BEGIN_REFLECTED` and `CXL_END_REFLECTED`/`CXL_EXT_END_REFLECTED`.
        + `TYPE` the type of the member
        + `INDEX` the index of the member, must start from 0 and be continuous.
        + Also some additional attributes can be defined:
            * `CXL_JSON_KEY(X)` attach a `json_key()` attribute, `X` is a string literal.
            * `CXL_XML_NAMESPACE(X)` attach a `xml_namespace()` attribute, `X` is a string literal.
            * `CXL_SQL_FIELD` attach a `sql_field()` attribute, `X` is a string literal.
    - `CXL_REFLECTED_MEMBER_KEY(INDEX, NAME, KEY, ...)` same as `CXL_REFLECTED_MEMBER`, but defines a `key()` attributes.
    - `CXL_REFLECTED_RO_MEMBER`, same as `CXL_REFLECTED_MEMBER`, but indicates the member is readonly.
    - `CXL_REFLECTED_RO_MEMBER_KEY` same as `CXL_REFLECTED_MEMBER_KEY`, but indicates the member is readonly.
    - `CXL_REFLECTED_ATTRIBUTE(INDEX, TYPE, NAME, GETTER, SETTER, ...)`, same as `CXL_REFLECTED_MEMBER`, but the attribute is get/set with `GETTER` and `SETTER`:
        + `CXL_MEM_GETTER(NAME)` use member function `NAME` as the getter.
        + `CXL_MEM_SETTER` use member function `NAME` as the setter
    - `CXL_REFLECTED_ATTRIBUTE_KEY` same as `CXL_REFLECTED_ATTRIBUTE`, but defines `key()` attribute.
    - `CXL_REFLECTED_RO_ATTRIBUTE` same as `CXL_REFLECTED_ATTRIBUTE`, but the member is readonly.
    - `CXL_REFLECTED_RO_ATTRIBUTE_KEY` same as `CXL_REFLECTED_ATTRIBUTE_KEY`, but the member is readonly.
* After metadata are defined, reflection support is added to the type:
    - `constexpr bool cxutil::reflectable<T>` is true if T supports reflection, all types with metadata defined are supports, also `std::tuple`, `std::pair`, and `std::array` are supported.
    - `to_variant_t<T>` is the corresponding `variant` type, used to dynamically access all members in T.
    - `tuple_size<T>` same as `std::tuple_size<T>`, but also supports reflectable types.
    - `tuple_element<I, T>` same as `std::tuple_element<I, T>`, but also supports reflectable types.
    - `get_variant<T>(i, t)` returns a `variant` contains the `i`-th memeber of `t`.
    - `get<Type>(i, t)` return the `i`-th member of `t` in type `Type`, if the `Type` is not the actual type of the member, a `std::bad_cast` exception will be thrown.
    - `set(i, t, v)` set the `i`-th member of `t` to `v`, if the type of `v` is not the actual type of the member, or the member is readonly, a `std::bad_cast` exception will be thrown.
    - `const char *get_name<T>()` return the name recorded in metadata of `T`.
    - `const char *get_sql_table<T>()` return the `sql_table()` attribute of `T`, returns to `name()` if the attribute doesn't exist.
    - `const char *get_xml_node<T>()` return the `xml_node()` attribute of `T`, returns to `name()` if the attribute doesn't exist.
    - `const char *get_xml_namespce<T>()` return the `xml_node()` attribute of `T`, returns `""` if the attribute doesn't exist.
    - `const char *get_element_name<T>(size_t i)` returns the name of `i`-th member of T.
    - `const char *get_element_key<T>(size_t i)` returns the attribute `key()` of `i`-th member of T, by default this attribute has same value with name.
    - `const char *get_element_sql_field<T>(size_t i)` returns the attribute `sql_field()` of `i`-th member of T, returns `key()` if the attribute doesn't exists.
    - `const char *get_element_json_key<T>(size_t i)` returns the attribute `json_key()` of `i`-th member of T, returns `key()` if the attribute doesn't exists.
    - `const char *get_element_xml_node<T>()` return the `xml_node()` attribute of `T`, returns `key()` if the attribute doesn't exist.
    - `const char *get_xml_namespace<T>()` return the `xml_node()` attribute of `T`, returns `""` if the attribute doesn't exist.
* Type with metadata defined supports `std::get`, and can be treated as a `std::tuple` in most scenarios.












