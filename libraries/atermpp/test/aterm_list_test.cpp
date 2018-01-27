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

  atermpp::aterm operator()(const atermpp::aterm& x) const
  {
    return read_term_from_string("f(" + pp(x) + ")");
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
    BOOST_CHECK(a + a == read_term_from_string("[1,2,3,1,2,3]"));
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

  aterm_list lm_union = term_list_union(l, m);
  BOOST_CHECK(lm_union.size() == 3);

  aterm_list lm_difference = term_list_difference(l, m);
  BOOST_CHECK(lm_difference.size() == 1);
}

void test_initializer_list()
{
  atermpp::aterm x = read_term_from_string("x");
  atermpp::aterm y = read_term_from_string("y");
  aterm_list l = { x, y };
}

void test_list_with_apply_filter()
{
  std::vector<aterm_list> v;
  aterm_list l1(read_term_from_string("[1,2,3,4]"));
  v.push_back(l1);
  aterm_list l2(read_term_from_string("[1,3,7]"));
  v.push_back(l2);
  aterm_list l3(read_term_from_string("[1,2,3,7,4]"));
  v.push_back(l3);
  aterm_list l4(read_term_from_string("[1,7]"));
  v.push_back(l4);
  aterm_list l5(read_term_from_string("[1,7,9,10,12,13,15,15,16]"));
  v.push_back(l5);

  // Remove the first element of each list. Only add resulting lists with a length larger than 2.
  // As a vector can be traversed backward, this tests the backward construction. 
  term_list<aterm_list> result1(v.begin(),
                                v.end(),
                                [](aterm_list l)->aterm_list{l.pop_front(); return l;},
                                [](const aterm_list& l)->bool{return l.size()>2;});
  BOOST_CHECK(result1.size()==3);
 
  // As a list can be traversed in a forward way, this tests the forward construction. 
  term_list<aterm_list> result2(result1.begin(),
                                result1.end(),
                                [](aterm_list l)->aterm_list{l.pop_front(); return l;},
                                [](const aterm_list& l)->bool{return l.size()>2;});
  BOOST_CHECK(result1.size()==2);
}

void test_concatenation()
{
  term_list<aterm_int> l1;
  term_list<aterm> l2;
  BOOST_CHECK(l2+l1 == l1+l2);
}

int test_main(int argc, char* argv[])
{
  test_aterm_list();
  test_set_operations();
  test_initializer_list();
  test_concatenation();

  return 0;
}
