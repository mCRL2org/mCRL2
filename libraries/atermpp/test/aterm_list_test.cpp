// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file aterm_list_test.cpp
/// \brief Add your file description here.

#include <sstream>
#include <algorithm>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/detail/utility.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/set_operations.h"

using namespace std;
using namespace atermpp;

struct counter
{
  int& m_sum;

  counter(int& sum)
    : m_sum(sum)
  {}

  void operator()(const atermpp::aterm& t)
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

  atermpp::aterm operator()(atermpp::aterm x) const
  {
    return read_term_from_string("f(" + to_string(x) + ")");
  }
};

void test_aterm_list()
{
  aterm_list q (read_term_from_string("[1,2,3,4]"));

  aterm_list r = reverse(q); // r == [4,3,2,1]
  BOOST_CHECK(r == read_term_from_string("[4,3,2,1]"));

  aterm_list r1 = q;
  r1=push_back<aterm>(r1,aterm_int(5));
  BOOST_CHECK(r1 == read_term_from_string("[1,2,3,4,5]"));

  atermpp::aterm f = q.front(); // f == 1
  BOOST_CHECK(f == aterm_int(1));

  q.push_front(read_term_from_string("[5,6]")); // q == [[5,6],1,2,3,4]

  stringstream os;
  for (aterm_list::iterator i = q.begin(); i != q.end(); ++i)
  {
    os << *i;
  }
  BOOST_CHECK(os.str() == "[5,6]1234");

  int sum = 0;
  std::for_each(r.begin(), r.end(), counter(sum));
  BOOST_CHECK(sum == 10);

  aterm_list v (read_term_from_string("[1,2,3,4]"));
  aterm_list w (read_term_from_string("[0,1,2,3,4]"));
  BOOST_CHECK(w.tail() == v);
  w.pop_front();
  BOOST_CHECK(w == v);
 
  // test concatenation
  {
    aterm_list a (read_term_from_string("[1,2,3]"));
    atermpp::aterm x = read_term_from_string("0");
    // BOOST_CHECK(x + a == read_term_from_string("[0,1,2,3]"));
    BOOST_CHECK(a + a == read_term_from_string("[1,2,3,1,2,3]"));
    // BOOST_CHECK(a + x == read_term_from_string("[1,2,3,0]"));
  }
}

void test_set_operations()
{
  atermpp::aterm x = read_term_from_string("x");
  atermpp::aterm y = read_term_from_string("y");
  atermpp::aterm z = read_term_from_string("z");

  aterm_list l;
  l.push_front(x);
  l.push_front(y);

  aterm_list m;
  m.push_front(z);
  m.push_front(x);

  // aterm_list lm_union = term_list_union(l, m);
  // BOOST_CHECK(lm_union.size() == 3);

  // aterm_list lm_difference = term_list_difference(l, m);
  // BOOST_CHECK(lm_difference.size() == 1);
}

int test_main(int argc, char* argv[])
{
  test_aterm_list();
  test_set_operations();

  return 0;
}
