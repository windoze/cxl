#include <assert.h>
#include <iostream>
#include <sstream>
#include <cxutil/type_traits.hpp>
#include <cxutil/variant.hpp>

using namespace cxutil;

void test_traits()
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
    static_assert(is_same<add_lref<int>, int&>, "xx");
    static_assert(is_same<add_lref<int&>, int&>, "xx");
    static_assert(is_same<add_lref<const int>, int const&>, "xx");
    static_assert(is_same<add_lref<const int*>, int const*&>, "xx");

    // add_rref
    static_assert(is_same<add_rref<int>, int&&>, "xx");
    static_assert(is_same<add_rref<int&>, int&>, "xx"); // (int&)&& = int&
    static_assert(is_same<add_rref<const int>, int const&&>, "xx");
    static_assert(is_same<add_rref<const int*>, int const*&&>, "xx");

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
}

void test_variant()
{
    variant<int, std::string> v(10);
    int n = get<int>(v);
    assert(n == 10);
    try {
        variant<int, std::string> v1("xyz");
        get<int>(v1); // bad_get
        assert(false);
    } catch (bad_get&) {
        assert(true);
    }
}

void test_rv()
{
    struct node;
    typedef variant<std::nullptr_t, int, recursive_wrapper<node>> node_data;
    struct node
    {
        node_data left;
        node_data right;
    } t;
    t.left = 10;
    t.right = node{10, 20};
}

void test_move()
{
    typedef variant<std::string, int> vt;
    vt v("hello");
    vt v1 = std::move(v);
    assert(v1.get<std::string>() == "hello");
    assert(v1.which() == 0);
    assert(v.which() == 0);
    // std::move clears std::string
    assert(v.get<std::string>().empty());
    vt v2 = 100;
    vt v3 = std::move(v2);
    assert(v3.get<int>() == 100);
    assert(v2.which() == 1);
    // std::move doesn't clear int
    assert(v2.get<int>() == 100);
}

void test_visitor()
{
    typedef variant<int, double> vt;
    struct visitor
    {
        std::string operator()(int x) && { return "int"; }
        std::string operator()(double x) && { return "double"; }
    };
    vt v(5.5);
    assert(v.apply_visitor(visitor()) == "double");
}

void test_print()
{
    typedef variant<int, std::string> vt;
    {
        vt v(100);
        std::stringstream ss;
        ss << v;
        assert(ss.str() == "100");
    }
    {
        vt v("hello");
        std::stringstream ss;
        ss << v;
        assert(ss.str() == "hello");
    }
}

struct iarchive
{
    iarchive(std::istream& is) : is_(is) {}
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, iarchive&>::type operator>>(T& n)
    {
        is_.read((char*)(&n), sizeof(n));
        return *this;
    }
    std::istream& is_;
};

struct oarchive
{
    oarchive(std::ostream& os) : os_(os) {}
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, oarchive&>::type operator<<(const T& n)
    {
        os_.write((const char*)(&n), sizeof(n));
        return *this;
    }
    std::ostream& os_;
};

void test_io()
{
    typedef variant<int, double> vt;
    {
        vt v(5.5);
        std::stringstream ss;
        oarchive oa(ss);
        write(oa, v);
        vt v1;
        iarchive ia(ss);
        read(ia, v1);
        assert(v == v1);
        assert(v1.get<double>() == 5.5);
    }
}

int main()
{
    test_traits();
    test_variant();
    test_rv();
    test_move();
    test_visitor();
    test_print();
    test_io();
    return 0;
}
