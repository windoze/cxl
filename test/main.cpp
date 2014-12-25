#include <iostream>
#include <assert.h>
#include <cxutil/type_traits.hpp>
#include <cxutil/variant.hpp>

using namespace cxutil;

int main()
{
    // is_same
    static_assert(is_same<int, int>, "xx");
    static_assert(!is_same<int, double>, "xx");
    static_assert(is_same<int, int, int, int>, "xx");
    static_assert(!is_same<int, int, int, double>, "xx");
    static_assert(!is_same<int, int, double, int>, "xx");
    static_assert(!is_same<double, int, int, int>, "xx");

    // unref
    static_assert(is_same<unref<int&>, int>, "xx");
    static_assert(is_same<unref<const int&>, int const>, "xx");
    static_assert(is_same<unref<int>, int>, "xx");
    static_assert(!is_same<unref<int*>, int>, "xx");

    // uncv
    static_assert(is_same<uncv<int const>, int>, "xx");
    static_assert(is_same<uncv<const int&>, const int&>, "xx");
    static_assert(is_same<uncv<int>, int>, "xx");
    static_assert(is_same<uncv<int&>, int&>, "xx");
    static_assert(is_same<uncv<const int*>, const int*>, "xx");
    static_assert(is_same<uncv<int* const>, int*>, "xx");
    static_assert(is_same<uncv<const int* const>, const int*>, "xx");

    // unrefcv
    static_assert(is_same<unrefcv<int const>, int>, "xx");
    static_assert(is_same<unrefcv<const int&>, int>, "xx");
    static_assert(is_same<unrefcv<int>, int>, "xx");
    static_assert(is_same<unrefcv<int>, int>, "xx");
    static_assert(is_same<unrefcv<const int*>, const int*>, "xx");
    static_assert(is_same<unrefcv<int* const>, int*>, "xx");
    static_assert(is_same<unrefcv<const int* const>, const int*>, "xx");

    // add_lref
    static_assert(is_same<add_lref<int>, int &>, "xx");
    static_assert(is_same<add_lref<int&>, int &>, "xx");
    static_assert(is_same<add_lref<const int>, int const &>, "xx");
    static_assert(is_same<add_lref<const int *>, int const *&>, "xx");

    // add_rref
    static_assert(is_same<add_rref<int>, int &&>, "xx");
    static_assert(is_same<add_rref<int&>, int &>, "xx");    // (int&)&& = int&
    static_assert(is_same<add_rref<const int>, int const &&>, "xx");
    static_assert(is_same<add_rref<const int *>, int const *&&>, "xx");

    // add_const
    static_assert(is_same<add_const<int>, int const>, "xx");
    static_assert(is_same<add_const<const int>, int const>, "xx");

    // add_volatile
    static_assert(is_same<add_volatile<int>, int volatile>, "xx");
    static_assert(is_same<add_volatile<volatile int>, int volatile>, "xx");

    // is_ref
    static_assert(is_ref<int&>, "xx");
    static_assert(is_ref<int&&>, "xx");
    static_assert(is_ref<const int&>, "xx");
    static_assert(!is_ref<int>, "xx");
    static_assert(!is_ref<int*>, "xx");

    // contained
    static_assert(contained<int, int, double>, "xx");
    static_assert(!contained<char, int, double>, "xx");

    typedef std::tuple<int, int, double> t3;
    static_assert(is_same<dedup<t3>::type, std::tuple<int, double>>, "xx");
    variant<int, std::string> v(10);
    int n = get<int>(v);
    assert(n == 10);
    std::string s("xxx");
    std::cout << s << std::endl;
    try {
        variant<int, std::string> v1("xyz");
        get<int>(v1);
    } catch (bad_get&) {
        assert(true);
    }
    variant<int, double>v2(10.5);
    assert(get<double>(v2)==10.5);
    return 0;
}
