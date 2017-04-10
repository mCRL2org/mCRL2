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

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_balanced_tree.h"
#include "mcrl2/atermpp/set_operations.h"

using namespace std;
using namespace atermpp;

struct counter
{
  int& m_sum;

  counter(int& sum)
    : m_sum(sum)
  {}

  void operator()(const atermpp::aterm& t) const
  {
    m_sum += aterm_int(t).value();
  }
};

struct increment
{
  atermpp::aterm operator()(const atermpp::aterm& t) const
  {
    return aterm_int(aterm_int(t).value() + 1);
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

static void test_aterm_balanced_tree()
{
  aterm_balanced_tree empty_tree;
  BOOST_CHECK(empty_tree.begin() == empty_tree.end());

  aterm_list q(read_term_from_string("[0,1,2,3,4,5,6,7,8,9]"));
  aterm_list r(read_term_from_string("[0,1,2,3,4,6,1,7,8,9]"));
  aterm_balanced_tree qtree(q.begin(),10);
  aterm_balanced_tree rtree(r.begin(),10);


  BOOST_CHECK(qtree.size() == 10);
  BOOST_CHECK(!qtree.empty());

  for (int i = 0; i != 10; ++i)
  {
    BOOST_CHECK(qtree[i] == aterm_int(i));
  }

  BOOST_CHECK(std::equal(qtree.begin(), qtree.end(), q.begin()));
  BOOST_CHECK(std::equal(q.begin(), q.end(), qtree.begin()));
  BOOST_CHECK(aterm_balanced_tree(q.begin(), q.size()) == qtree);

  int count = 0;

  std::for_each(qtree.begin(), qtree.end(), counter(count));

  BOOST_CHECK(count == 45);

  count = 0;

  std::for_each(qtree.begin(), qtree.end(), counter(count));

  aterm_balanced_tree qcopy(q.begin(),10);

  qtree.swap(qcopy);

  BOOST_CHECK(std::equal(qtree.begin(), qtree.end(), q.begin()));
  BOOST_CHECK(std::equal(q.begin(), q.end(), qtree.begin()));

  BOOST_CHECK(!std::equal(rtree.begin(), rtree.end(), q.begin()));
  BOOST_CHECK(!std::equal(q.begin(), q.end(), rtree.begin()));
} 

int test_main(int , char**)
{
  test_aterm_balanced_tree(); 

  return 0;
}
