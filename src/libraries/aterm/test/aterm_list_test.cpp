// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : test/aterm_list_test.cpp
// date          : 04/25/05
// version       : 0.3
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#include <sstream>
#include <algorithm>
#include <boost/test/minimal.hpp>

#include "atermpp/aterm.h"
#include "atermpp/aterm_int.h"
#include "atermpp/aterm_list.h"
#include "atermpp/transform.h"

using namespace std;
using namespace atermpp;

struct counter
{
  int& m_sum;

  counter(int& sum)
    : m_sum(sum)
  {}

  void operator()(const aterm& t)
  {
    m_sum += aterm_int(t).value();
  }
};

struct func
{
  func()
  {}

  func(int)
  {}

  aterm operator()(aterm x) const
  {
    return make_term("f(" + x.to_string() + ")");
  }
};

void test_aterm_list()
{
  aterm_list q = make_term("[1,2,3,4]");

  aterm_list r = reverse(q); // r == [4,3,2,1]
  BOOST_CHECK(r == make_term("[4,3,2,1]")); 

  aterm_list r1 = push_back(q, aterm(aterm_int(5)));
  BOOST_CHECK(r1 == make_term("[1,2,3,4,5]")); 

  aterm f = q.front(); // f == 1
  BOOST_CHECK(f == aterm_int(1));

  q = push_front(q, make_term("[5,6]")); // q == [[5,6],1,2,3,4]

  stringstream os;
  for (aterm_list::iterator i = q.begin(); i != q.end(); ++i)
  {
    os << *i;
  }
  BOOST_CHECK(os.str() == "[5,6]1234");

  int sum = 0;
  for_each(r.begin(), r.end(), counter(sum));
  BOOST_CHECK(sum == 10);
  
  aterm_list v = make_term("[1,2,3,4]");
  aterm_list w = make_term("[0,1,2,3,4]");
  BOOST_CHECK(pop_front(w) == v);
}

int test_main( int, char*[] )
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  test_aterm_list();
  return 0;
}
