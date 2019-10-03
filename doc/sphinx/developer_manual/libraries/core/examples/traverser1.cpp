//[traverser1
#include <cassert>
#include <vector>
#include "mcrl2/core/traverser.h"

using namespace mcrl2;

struct int_traverser: public core::traverser<int_traverser>
{
  // The following lines are needed to make static polymorphism work.
  typedef core::traverser<int_traverser> super;
  using super::operator();
  using super::enter;
  using super::leave;
  
  int count;
  
  int_traverser()
    : count(0)
  {}
  
  // Handle integers.
  void operator()(int i)
  {
    count += i;
  }
};

int main()
{
  std::vector<int> v;
  v.push_back(1);
  v.push_back(4);

  int_traverser f;
  f(v);
  assert(f.count == 5);
  
  return 0;
}
//]