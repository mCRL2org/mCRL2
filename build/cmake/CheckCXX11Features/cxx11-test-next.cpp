#include <iterator>
#include <vector>
#include <iostream>

int main()
{
  std::vector<int> v;
  v.push_back(0);
  v.push_back(1);
  std::vector<int>::iterator i = v.begin();
  std::cout << *std::next(i) << " comes after " << *i << std::endl;
  return 0;
}
