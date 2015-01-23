#include <iostream>
#include <type_traits>

class A {};
class B : public A {};
class C {};
 
int main() 
{
    std::cout << std::boolalpha;
    std::cout << std::is_convertible<B*, A*>::value << '\n';
    std::cout << std::is_convertible<A*, B*>::value << '\n';
    std::cout << std::is_convertible<B*, C*>::value << '\n';
    return 0;
}
