#include <vector>

int main()
{
  std::vector<int> v;
  v.push_back(1);
  int sum = 0;
  for (int i: v)
  {
    sum = sum + i;
  }
  return 0;
}
