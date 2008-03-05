#include <numeric>
#include <vector>
#include <boost/test/minimal.hpp>
#include "mcrl2/core/sequence.h"

using namespace mcrl2;

struct f
{
  int& sum_;

  f(int& sum)
    : sum_(sum)
  {}

  template <typename Iter>
  void operator()(Iter first, Iter last) const
  {
    for (Iter i = first; i != last; ++i)
    {
      std::cout << *i << " ";
    }
    std::cout << std::endl;
    sum_ += std::accumulate(first, last, 0);
  }
};

void test_sequence()
{
  std::vector<int> a;
  std::vector<int> b;
  std::vector<int> c;

  a.push_back(1);
  a.push_back(2);
  b.push_back(3);
  c.push_back(2);
  c.push_back(5);
  
  std::vector<std::vector<int> > v;
  v.push_back(a);
  v.push_back(b);
  v.push_back(c);
  
  int sum = 0;
  core::foreach_sequence(v, f(sum));
  std::cout << "sum == " << sum << std::endl;
  BOOST_CHECK(sum == 32);
}

int test_main(int, char*[])
{
  test_sequence();
  
  return 0;
}
