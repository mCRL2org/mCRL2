#include <type_traits>

int main()
{
  std::remove_const<const int>::type i = 1;
  i = 0;
  return i;
}
