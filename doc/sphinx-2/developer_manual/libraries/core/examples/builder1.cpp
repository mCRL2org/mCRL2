//[builder1
#include <cassert>
#include <vector>
#include "mcrl2/core/builder.h"

using namespace mcrl2;

struct int_builder: public core::builder<int_builder>
{
  // The following lines are needed to make static polymorphism work.
  typedef core::builder<int_builder> super;
  using super::operator();
  using super::enter;
  using super::leave;

  // Handle integers.
  void operator()(int& i)
  {
    i += 1;
  }
};

int main()
{
  std::vector<int> v;
  v.push_back(1);
  v.push_back(4);

  int_builder f;
  f(v);
  assert(v[0] == 2);
  assert(v[1] == 5);
  
  return 0;
}
//]