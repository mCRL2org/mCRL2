#include <initializer_list>
#include <vector>

struct A
{
  A(std::initializer_list<int> x)
  { }
};

int main()
{
  std::vector<int> v = { 1, 2, 3 };
  A a({ 1, 2, 3 });
  return 0;
}
