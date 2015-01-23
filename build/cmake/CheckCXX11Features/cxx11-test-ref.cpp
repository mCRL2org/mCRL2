#include <functional>

int main()
{
  int i = 1;
  std::reference_wrapper<int> j = std::ref(i);
  j -= 1;
  return i;
}
