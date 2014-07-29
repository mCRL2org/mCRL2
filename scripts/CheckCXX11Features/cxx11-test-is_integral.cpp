#include <iostream>
#include <string>
#include <type_traits>
 
int main() 
{
    std::cout << std::boolalpha;
    std::cout << "string: " << std::is_integral<std::string>::value << '\n';
    std::cout << "integer: " << std::is_integral<int>::value << '\n';
    return 0;
}
