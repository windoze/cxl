#include <assert.h>
#include <iostream>
#include <sstream>
#include <cxutil/variant.hpp>
#include <cxutil/type_traits.hpp>
#include <cxutil/stdio_filebuf.hpp>
#include <cxutil/str_lit.hpp>
#include <fcntl.h>
#include <cxutil/reflection.hpp>

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)
#define POS __FILE__ ":" LINE_STRING

using namespace cxutil;

// is_same

#if __cplusplus > 201103L
// C++14 features
static_assert(is_same<int, int>, POS);
static_assert(!is_same<int, double>, POS);
static_assert(is_same<int, int, int, int>, POS);
static_assert(!is_same<int, int, int, double>, POS);
static_assert(!is_same<int, int, double, int>, POS);
static_assert(!is_same<double, int, int, int>, POS);

// unref
static_assert(is_same<unref<int&>, int>, POS);
static_assert(is_same<unref<const int&>, int const>, POS);
static_assert(is_same<unref<int>, int>, POS);
static_assert(!is_same<unref<int*>, int>, POS);

// uncv
static_assert(is_same<uncv<int const>, int>, POS);
static_assert(is_same<uncv<const int&>, const int&>, POS);
static_assert(is_same<uncv<int>, int>, POS);
static_assert(is_same<uncv<int&>, int&>, POS);
static_assert(is_same<uncv<const int*>, const int*>, POS);
static_assert(is_same<uncv<int* const>, int*>, POS);
static_assert(is_same<uncv<const int* const>, const int*>, POS);

// unrefcv
static_assert(is_same<unrefcv<int const>, int>, POS);
static_assert(is_same<unrefcv<const int&>, int>, POS);
static_assert(is_same<unrefcv<int>, int>, POS);
static_assert(is_same<unrefcv<int>, int>, POS);
static_assert(is_same<unrefcv<const int*>, const int*>, POS);
static_assert(is_same<unrefcv<int* const>, int*>, POS);
static_assert(is_same<unrefcv<const int* const>, const int*>, POS);

// add_lref
static_assert(is_same<add_lref<int>, int&>, POS);
static_assert(is_same<add_lref<int&>, int&>, POS);
static_assert(is_same<add_lref<const int>, int const&>, POS);
static_assert(is_same<add_lref<const int*>, int const*&>, POS);

// add_rref
static_assert(is_same<add_rref<int>, int&&>, POS);
static_assert(is_same<add_rref<int&>, int&>, POS); // (int&)&& = int&
static_assert(is_same<add_rref<const int>, int const&&>, POS);
static_assert(is_same<add_rref<const int*>, int const*&&>, POS);

// add_const
static_assert(is_same<add_const<int>, int const>, POS);
static_assert(is_same<add_const<const int>, int const>, POS);

// add_volatile
static_assert(is_same<add_volatile<int>, int volatile>, POS);
static_assert(is_same<add_volatile<volatile int>, int volatile>, POS);

// is_ref
static_assert(is_ref<int&>, POS);
static_assert(is_ref<int&&>, POS);
static_assert(is_ref<const int&>, POS);
static_assert(!is_ref<int>, POS);
static_assert(!is_ref<int*>, POS);

// contained
static_assert(contained<int, int, double>, POS);
static_assert(!contained<char, int, double>, POS);

// dedup tuple
typedef std::tuple<int, int, double> t3;
static_assert(is_same<dedup<t3>::type, std::tuple<int, double>>, POS);

// dedup variant
static_assert(is_same<deduped_variant<int, int, double>, variant<int, double>>, POS);
static_assert(is_same<deduped_variant<int, double>, variant<int, double>>, POS);
static_assert(is_same<deduped_variant<int, int, double, double>, variant<int, double>>, POS);
static_assert(is_same<deduped_variant<int, int, double, double, int, int>, variant<double, int>>,
              POS);

// type_index
static_assert(is_same<std::tuple_element<typeindex<int, std::tuple<char, int, double>>,
                                         std::tuple<char, int, double>>::type,
                      int>,
              POS);
#else
// C++11 version
static_assert(is_same_t<int, int>::value, POS);
static_assert(!is_same_t<int, double>::value, POS);
static_assert(is_same_t<int, int, int, int>::value, POS);
static_assert(!is_same_t<int, int, int, double>::value, POS);
static_assert(!is_same_t<int, int, double, int>::value, POS);
static_assert(!is_same_t<double, int, int, int>::value, POS);

// unref
static_assert(is_same_t<unref<int&>, int>::value, POS);
static_assert(is_same_t<unref<const int&>, int const>::value, POS);
static_assert(is_same_t<unref<int>, int>::value, POS);
static_assert(!is_same_t<unref<int*>, int>::value, POS);

// uncv
static_assert(is_same_t<uncv<int const>, int>::value, POS);
static_assert(is_same_t<uncv<const int&>, const int&>::value, POS);
static_assert(is_same_t<uncv<int>, int>::value, POS);
static_assert(is_same_t<uncv<int&>, int&>::value, POS);
static_assert(is_same_t<uncv<const int*>, const int*>::value, POS);
static_assert(is_same_t<uncv<int* const>, int*>::value, POS);
static_assert(is_same_t<uncv<const int* const>, const int*>::value, POS);

// unrefcv
static_assert(is_same_t<unrefcv<int const>, int>::value, POS);
static_assert(is_same_t<unrefcv<const int&>, int>::value, POS);
static_assert(is_same_t<unrefcv<int>, int>::value, POS);
static_assert(is_same_t<unrefcv<int>, int>::value, POS);
static_assert(is_same_t<unrefcv<const int*>, const int*>::value, POS);
static_assert(is_same_t<unrefcv<int* const>, int*>::value, POS);
static_assert(is_same_t<unrefcv<const int* const>, const int*>::value, POS);

// add_lref
static_assert(is_same_t<add_lref<int>, int&>::value, POS);
static_assert(is_same_t<add_lref<int&>, int&>::value, POS);
static_assert(is_same_t<add_lref<const int>, int const&>::value, POS);
static_assert(is_same_t<add_lref<const int*>, int const*&>::value, POS);

// add_rref
static_assert(is_same_t<add_rref<int>, int&&>::value, POS);
static_assert(is_same_t<add_rref<int&>, int&>::value, POS); // (int&)&& = int&
static_assert(is_same_t<add_rref<const int>, int const&&>::value, POS);
static_assert(is_same_t<add_rref<const int*>, int const*&&>::value, POS);

// add_const
static_assert(is_same_t<add_const<int>, int const>::value, POS);
static_assert(is_same_t<add_const<const int>, int const>::value, POS);

// add_volatile
static_assert(is_same_t<add_volatile<int>, int volatile>::value, POS);
static_assert(is_same_t<add_volatile<volatile int>, int volatile>::value, POS);

// contained
static_assert(contained_t<int, int, double>::value, POS);
static_assert(!contained_t<char, int, double>::value, POS);

// dedup tuple
typedef std::tuple<int, int, double> t3;
static_assert(is_same_t<dedup<t3>::type, std::tuple<int, double>>::value, POS);

// dedup variant
static_assert(is_same_t<deduped_variant<int, int, double>, variant<int, double>>::value, POS);
static_assert(is_same_t<deduped_variant<int, double>, variant<int, double>>::value, POS);
static_assert(is_same_t<deduped_variant<int, int, double, double>, variant<int, double>>::value,
              POS);
static_assert(
    is_same_t<deduped_variant<int, int, double, double, int, int>, variant<double, int>>::value,
    POS);

// type_index
static_assert(is_same_t<std::tuple_element<type_index<int, std::tuple<char, int, double>>::value,
                                           std::tuple<char, int, double>>::type,
                        int>::value,
              POS);
#endif

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

void test_recursive_variant()
{
    struct node;
    typedef variant<std::nullptr_t, int, recursive_wrapper<node>> node_data;
    struct node {
        node_data left;
        node_data right;
    } t;
    t.left = 5;
    t.right = node{10, 20};
    assert(t.left.get<int>() == 5);
    assert(t.right.get<node>().right.get<int>() == 20);
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
    struct visitor {
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

struct iarchive {
    iarchive(std::istream& is) : is_(is) {}
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, iarchive&>::type operator>>(T& n)
    {
        is_.read((char*)(&n), sizeof(n));
        return *this;
    }
    std::istream& is_;
};

struct oarchive {
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

void test_filebuf() {
    int fd=open("/tmp/x", O_CREAT | O_RDWR, 0644);
    stdio_filebuf<char> *fb=new stdio_filebuf<char>;
    fb->open(fd, std::ios::in | std::ios::out);
    std::iostream s(fb);
    s << "abc" << std::endl;
}

#include "main.hpp"
#include "main_reflected.hpp"

void test_reflected() {
    S s{420, 4.2, "hello", {4200}};
    // Test cxutil::reflection::set
    try {
        // This will succeed
        cxutil::reflection::set("m1", s, 210);
        // This will fail
        cxutil::reflection::set(1, s, "210");
        assert(false);
    } catch(cxutil::bad_get &) {
    }
    assert(s.m1==210);

    try {
        // This will fail, index out of range
        cxutil::reflection::set(10, s, 55);
        assert(false);
    } catch(std::out_of_range &) {
    }

    try {
        // This will fail, unknown key
        cxutil::reflection::set("unknown key", s, 55);
        assert(false);
    } catch(std::out_of_range &) {
    }

    // Test std::get
    std::get<2>(s)="world";
    assert(s.m3==std::string("world"));
    // Test cxutil::reflection::get
    assert(cxutil::reflection::get(2, s).get<std::string>()==std::string("world"));
}

int main()
{
    test_variant();
    test_recursive_variant();
    test_move();
    test_visitor();
    test_print();
    test_io();
    test_filebuf();
    test_reflected();
    return 0;
}
