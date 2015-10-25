#ifndef CXL_REFLECTION_HPP
#define CXL_REFLECTION_HPP

#include <cxl/reflection/reflection_impl.hpp>

#define CXL_PP_CONCAT9(a, b, c, d, e, f, g, h, i) a b c d e f g h i
#define CXL_PP_CONCAT8(a, b, c, d, e, f, g, h) a b c d e f g h
#define CXL_PP_CONCAT7(a, b, c, d, e, f, g) a b c d e f g
#define CXL_PP_CONCAT6(a, b, c, d, e, f) a b c d e f
#define CXL_PP_CONCAT5(a, b, c, d, e) a b c d e
#define CXL_PP_CONCAT4(a, b, c, d) a b c d
#define CXL_PP_CONCAT3(a, b, c) a b c
#define CXL_PP_CONCAT2(a, b) a b
#define CXL_PP_CONCAT1(a) a
#define CXL_PP_GET_OVERRIDE(_1, _2, _3, _4, _5, _6, _7, _8, _9, NAME, ...) NAME
#define CXL_PP_CONCAT(...)                                                                         \
    CXL_PP_GET_OVERRIDE(__VA_ARGS__, CXL_PP_CONCAT9, CXL_PP_CONCAT8, CXL_PP_CONCAT7, CXL_PP_CONCAT6, CXL_PP_CONCAT5, CXL_PP_CONCAT4, CXL_PP_CONCAT3, CXL_PP_CONCAT2, CXL_PP_CONCAT1)(__VA_ARGS__)

#define CXL_BEGIN_REFLECTED(TYPE, COUNT, ARGS...)                                                  \
public:                                                                                            \
    struct cxl_reflected_metadata {                                                                \
        CXL_PP_CONCAT(ARGS);                                                                       \
        static constexpr bool enabled = true;                                                      \
        typedef TYPE owner_type;                                                                   \
        static constexpr char const* name() { return #TYPE; }                                      \
        static constexpr std::size_t element_count = COUNT;                                        \
        template <size_t I, typename Unused>                                                       \
        struct element;
#define CXL_END_REFLECTED(ARGS...)                                                                 \
    ARGS;                                                                                          \
    }                                                                                              \
    ;

// Can be used out of struct, must be put in global namespace
#define CXL_EXT_BEGIN_REFLECTED(TYPE, COUNT, ARGS...)                                              \
    namespace cxl                                                                               \
    {                                                                                              \
    namespace reflection                                                                           \
    {                                                                                              \
        namespace metadata_store                                                                   \
        {                                                                                          \
            template <>                                                                            \
            struct cxl_reflected_metadata<TYPE> {                                                  \
                CXL_PP_CONCAT(ARGS);                                                               \
                static constexpr bool enabled = true;                                              \
                typedef TYPE owner_type;                                                           \
                static constexpr char const* name() { return #TYPE; }                              \
                static constexpr unsigned long element_count = COUNT;                              \
                template <size_t I, typename Unused>                                               \
                struct element;
#define CXL_EXT_END_REFLECTED(ARGS...)                                                             \
    CXL_PP_CONCAT(ARGS);                                                                           \
    }                                                                                              \
    ;                                                                                              \
    }}} /* End of namespace cxl::reflection::metadata_store */

#define CXL_REFLECTED_MEMBER(INDEX, NAME, ARGS...)                                                 \
    template <typename Unused>                                                                     \
    struct element<INDEX, Unused> {                                                                \
        static constexpr bool use_value = false;                                                   \
        static constexpr const unsigned long index = INDEX;                                        \
        typedef decltype(owner_type::NAME) type;                                                   \
        static constexpr char const* name() { return #NAME; }                                      \
        static constexpr char const* key() { return #NAME; }                                       \
        template <typename Owner>                                                                  \
        static constexpr std::enable_if_t<!std::is_const<decltype(Owner::NAME)>::value, type>&     \
        get(Owner& d)                                                                              \
        {                                                                                          \
            return d.NAME;                                                                         \
        }                                                                                          \
        template <typename Owner>                                                                  \
        static constexpr std::enable_if_t<!std::is_const<decltype(Owner::NAME)>::value, type>&&    \
        get(Owner&& d)                                                                             \
        {                                                                                          \
            return static_cast<type&&>(d.NAME);                                                    \
        }                                                                                          \
        static constexpr type const& get(owner_type const& d) { return d.NAME; }                   \
        template <typename Owner, typename V>                                                      \
        static std::enable_if_t<!std::is_const<decltype(Owner::NAME)>::value> set(Owner& d, V&& v) \
        {                                                                                          \
            d.NAME = std::forward<V>(v);                                                           \
        }                                                                                          \
        CXL_PP_CONCAT(ARGS);                                                                       \
    };

#define CXL_REFLECTED_MEMBER_KEY(INDEX, NAME, KEY, ARGS...)                                        \
    template <typename Unused>                                                                     \
    struct element<INDEX, Unused> {                                                                \
        static constexpr bool use_value = false;                                                   \
        static constexpr const unsigned long index = INDEX;                                        \
        typedef decltype(owner_type::NAME) type;                                                   \
        static constexpr char const* name() { return #NAME; }                                      \
        static constexpr char const* key() { return KEY; }                                         \
        template <typename Owner>                                                                  \
        static constexpr std::enable_if_t<!std::is_const<decltype(Owner::NAME)>::value, type>&     \
        get(Owner& d)                                                                              \
        {                                                                                          \
            return d.NAME;                                                                         \
        }                                                                                          \
        template <typename Owner>                                                                  \
        static constexpr std::enable_if_t<!std::is_const<decltype(Owner::NAME)>::value, type>&&    \
        get(Owner&& d)                                                                             \
        {                                                                                          \
            return static_cast<type&&>(d.NAME);                                                    \
        }                                                                                          \
        static constexpr type const& get(owner_type const& d) { return d.NAME; }                   \
        template <typename Owner, typename V>                                                      \
        static std::enable_if_t<!std::is_const<decltype(Owner::NAME)>::value> set(Owner& d, V&& v) \
        {                                                                                          \
            d.NAME = std::forward<V>(v);                                                           \
        }                                                                                          \
        CXL_PP_CONCAT(ARGS);                                                                       \
    };

#define CXL_REFLECTED_RO_MEMBER(INDEX, NAME, ARGS...)                                              \
    template <typename Unused>                                                                     \
    struct element<INDEX, Unused> {                                                                \
        static constexpr bool use_value = false;                                                   \
        static constexpr const unsigned long index = INDEX;                                        \
        typedef decltype(owner_type::NAME) type;                                                   \
        static constexpr char const* name() { return #NAME; }                                      \
        static constexpr char const* key() { return #NAME; }                                       \
        static constexpr type const& get(owner_type const& d) { return d.NAME; }                   \
        CXL_PP_CONCAT(ARGS);                                                                       \
    };

#define CXL_REFLECTED_RO_MEMBER_KEY(INDEX, NAME, KEY, ARGS...)                                     \
    template <typename Unused>                                                                     \
    struct element<INDEX, Unused> {                                                                \
        static constexpr bool use_value = false;                                                   \
        static constexpr const unsigned long index = INDEX;                                        \
        typedef decltype(owner_type::NAME) type;                                                   \
        static constexpr char const* name() { return #NAME; }                                      \
        static constexpr char const* key() { return KEY; }                                         \
        static constexpr type const& get(owner_type const& d) { return d.NAME; }                   \
        CXL_PP_CONCAT(ARGS);                                                                       \
    };

// Use like: CXL_REFLECTED_ATTRIBUTE(3, int, attr4, CXL_MEM_GETTER(get_attr4),
// CXL_MEM_SETTER(set_attr4))
#define CXL_REFLECTED_ATTRIBUTE(INDEX, TYPE, NAME, GETTER, SETTER, ARGS...)                        \
    template <typename Unused>                                                                     \
    struct element<INDEX, Unused> {                                                                \
        static constexpr bool use_value = true;                                                    \
        static constexpr const unsigned long index = INDEX;                                        \
        typedef TYPE type;                                                                         \
        static constexpr char const* name() { return #NAME; }                                      \
        static constexpr char const* key() { return #NAME; }                                       \
        static constexpr type get(owner_type const& d) { return GETTER; }                          \
        template <typename V>                                                                      \
        static void set(owner_type& d, V&& v)                                                      \
        {                                                                                          \
            SETTER;                                                                                \
        }                                                                                          \
        CXL_PP_CONCAT(ARGS);                                                                       \
    };

#define CXL_REFLECTED_ATTRIBUTE_KEY(INDEX, TYPE, NAME, KEY, GETTER, SETTER, ARGS...)               \
    template <typename Unused>                                                                     \
    struct element<INDEX, Unused> {                                                                \
        static constexpr bool use_value = true;                                                    \
        static constexpr const unsigned long index = INDEX;                                        \
        typedef TYPE type;                                                                         \
        static constexpr char const* name() { return #NAME; }                                      \
        static constexpr char const* key() { return KEY; }                                         \
        static constexpr type get(owner_type const& d) { return GETTER; }                          \
        template <typename V>                                                                      \
        static void set(owner_type& d, V&& v)                                                      \
        {                                                                                          \
            SETTER;                                                                                \
        }                                                                                          \
        CXL_PP_CONCAT(ARGS);                                                                       \
    };

#define CXL_REFLECTED_RO_ATTRIBUTE(INDEX, TYPE, NAME, GETTER, ARGS...)                             \
    template <typename Unused>                                                                     \
    struct element<INDEX, Unused> {                                                                \
        static constexpr bool use_value = true;                                                    \
        static constexpr const unsigned long index = INDEX;                                        \
        typedef typename std::add_const<TYPE>::type type;                                          \
        static constexpr char const* name() { return #NAME; }                                      \
        static constexpr char const* key() { return #NAME; }                                       \
        static constexpr type get(owner_type const& d) { return GETTER; }                          \
        CXL_PP_CONCAT(ARGS);                                                                       \
    };

#define CXL_REFLECTED_RO_ATTRIBUTE_KEY(INDEX, TYPE, NAME, KEY, GETTER, ARGS...)                    \
    template <typename Unused>                                                                     \
    struct element<INDEX, Unused> {                                                                \
        static constexpr bool use_value = true;                                                    \
        static constexpr const unsigned long index = INDEX;                                        \
        typedef typename std::add_const<TYPE>::type type;                                          \
        static constexpr char const* name() { return #NAME; }                                      \
        static constexpr char const* key() { return KEY; }                                         \
        static constexpr type get(owner_type const& d) { return GETTER; }                          \
        CXL_PP_CONCAT(ARGS);                                                                       \
    };

#define CXL_MEM_GETTER(GETTER) d.GETTER()
#define CXL_MEM_SETTER(SETTER) d.SETTER(std::forward<V>(v))

#define CXL_ATTR(NAME, VALUE)                                                                      \
    static constexpr char const* NAME() { return VALUE; }

#define CXL_SQL_TABLE(X) CXL_ATTR(sql_table, X)

#define CXL_JSON_KEY(X) CXL_ATTR(json_key, X)

#define CXL_XML_NODE(X) CXL_ATTR(xml_node, X)

#define CXL_XML_NAMESPACE(X) CXL_ATTR(xml_namespace, X)

#define CXL_SQL_FIELD(X) CXL_ATTR(sql_field, X)

#define CXL_CSV_FIELD(X) CXL_ATTR(csv_field, X)

#endif // !defined(CXL_REFLECTION_HPP)
