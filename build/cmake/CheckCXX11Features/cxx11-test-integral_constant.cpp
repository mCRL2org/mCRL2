#include <iostream>
#include <type_traits>

int main()
{
  std::cout << std::boolalpha;
  std::cout << std::true_type::value << std::endl;
  std::cout << std::false_type::value << std::endl;
  std::cout << std::integral_constant<int, 10>::value << std::endl;
  return 0;
}
