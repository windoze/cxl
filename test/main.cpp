#include <iostream>
#include <assert.h>
#include <cxutil/type_traits.hpp>
#include <cxutil/variant.hpp>

using namespace std;

int main()
{
    using namespace utility;
    static_assert(contained<int, int, double>, "xx");
    static_assert(!contained<char, int, double>, "xx");
    typedef std::tuple<int, int, double> t3;
    static_assert(utility::is_same<dedup<t3>::type, std::tuple<int, double>>, "xx");
    variant<int, std::string> v(10);
    int n = get<int>(v);
    assert(n == 10);
    try {
        variant<int, std::string> v1("xyz");
        int n1 = get<int>(v1);
        assert(false);
    } catch (bad_get&) {
    }
    cout << "Hello World!" << endl;
    return 0;
}
