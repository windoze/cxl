#ifndef MAIN_HPP
#define MAIN_HPP

#include <string>
#include <ostream>
#include <cxl/reflection.hpp>

struct S_inner
{
    int m1;
};

inline std::ostream &operator<<(std::ostream &os, const S_inner &s)
{
    os << "S_inner{" << s.m1 << "}";
    return os;
}

struct S
{
    int m1;
    double m2;
    std::string m3;
    S_inner m4;
};

inline std::ostream &operator<<(std::ostream &os, const S &s)
{
    os << "S{" << s.m1 << "," << s.m2 << ",\"" << s.m3 << "\"," << s.m4 << "}";
    return os;
}

struct SC
{
    int m1;
    const double m2;

    SC(int mm1, double mm2, int mm3) : m1(mm1), m2(mm2), m3(mm3) { }

    int get_m3() const { return m3; }

    void set_m3(int x) { m3 = x; }

    int get_m4() const { return 100; }

private:
    int m3;
    // Metadata within the struct
    /* clang-format off */
    /* @formatter:off */
    CXL_BEGIN_REFLECTED(SC, 4)
        CXL_REFLECTED_MEMBER(0, m1)
        CXL_REFLECTED_MEMBER(1, m2)
        CXL_REFLECTED_ATTRIBUTE(2, int, m3, CXL_MEM_GETTER(get_m3), CXL_MEM_SETTER(set_m3))
        CXL_REFLECTED_RO_ATTRIBUTE(3, int, m4, CXL_MEM_GETTER(get_m4))
    CXL_END_REFLECTED()
    /* @formatter:on */
    /* clang-format on */
};

// Metadata out of the struct
/* clang-format off */
/* @formatter:off */
CXL_EXT_BEGIN_REFLECTED(S_inner, 1)
    CXL_REFLECTED_MEMBER(0, m1)
CXL_EXT_END_REFLECTED()
CXL_EXT_BEGIN_REFLECTED(S, 4, CXL_SQL_TABLE("some_table"))
    CXL_REFLECTED_MEMBER(0, m1)
    CXL_REFLECTED_MEMBER(1, m2, CXL_SQL_FIELD("field2"), CXL_XML_NAMESPACE("somens"))
    CXL_REFLECTED_MEMBER_KEY(2, m3, "MM3")
    CXL_REFLECTED_MEMBER(3, m4)
CXL_EXT_END_REFLECTED()
/* @formatter:on */
/* clang-format on */

#endif // MAIN_HPP
