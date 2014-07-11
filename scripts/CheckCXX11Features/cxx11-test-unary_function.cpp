#include <iostream>
#include <functional>

struct functor : std::unary_function<int, bool>
{
  bool operator()(int i) const { return i < 7; }
};

int main()
{
  std::cout << std::boolalpha << functor()(12) << std::endl;
  return 0;
}
