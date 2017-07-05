#include <iostream>
#include <type_traits>

int main()
{
  typedef std::make_unsigned<int>::type u_type;
  u_type i = (u_type)-1;
  int j = (int)-1;
  std::cout << "i = " << i << std::endl;
  std::cout << "j = " << j << std::endl;
  return 0;
}
