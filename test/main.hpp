#ifndef MAIN_HPP
#define MAIN_HPP

#include <string>
#include <ostream>
#include <cxutil/reflection.hpp>

struct CXL_REFLECTABLE S_inner {
    int m1;
};

inline std::ostream &operator<<(std::ostream &os, const S_inner &s) {
    os << "S_inner{" << s.m1 << "}";
    return os;
}

struct CXL_REFLECTABLE S {
    int m1;
    double m2;
    std::string CXL_KEY("MM3") m3;
    S_inner m4;
};

inline std::ostream &operator<<(std::ostream &os, const S &s) {
    os << "S{" << s.m1 << "," << s.m2 << ",\"" << s.m3 << "\"," << s.m4 << "}";
    return os;
}

#endif	// MAIN_HPP
