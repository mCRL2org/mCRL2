// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file aterm_list_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE aterm_list_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_balanced_tree.h"
#include "mcrl2/atermpp/set_operations.h"

using namespace atermpp;

struct counter
{
  int& m_sum;

  counter(int& sum)
    : m_sum(sum)
  {}

  void operator()(const atermpp::aterm& t) const
  {
    m_sum += down_cast<aterm_int>(t).value();
  }
};

struct increment
{
  atermpp::aterm operator()(const atermpp::aterm& t) const
  {
    return aterm_int(down_cast<aterm_int>(t).value() + 1);
  }
};

struct func
{
  func() = default;

  func(int)
  {}

  atermpp::aterm operator()(const atermpp::aterm& x) const
  {
    return read_term_from_string("f(" + pp(x) + ")");
  }
};

BOOST_AUTO_TEST_CASE(test_aterm_balanced_tree)
{
  aterm_balanced_tree empty_tree;
  BOOST_CHECK(empty_tree.begin() == empty_tree.end());

  aterm_list q = read_list_from_string("[0,1,2,3,4,5,6,7,8,9]");
  aterm_list r(read_list_from_string("[0,1,2,3,4,6,1,7,8,9]"));
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

BOOST_AUTO_TEST_CASE(test_singleton_aterm_balanced_tree)
{
  aterm_balanced_tree empty_tree;
  BOOST_CHECK(empty_tree.begin() == empty_tree.end());

  aterm_list q = read_list_from_string("[7]");
  aterm_list r(read_list_from_string("[0]"));
  aterm_balanced_tree qtree(q.begin(),1);
  aterm_balanced_tree rtree(r.begin(),1);


  BOOST_CHECK(qtree.size() == 1);
  BOOST_CHECK(!qtree.empty());

  for (int i = 0; i != 1; ++i)
  {
    BOOST_CHECK(qtree[i] == aterm_int(7));
  }

  BOOST_CHECK(std::equal(qtree.begin(), qtree.end(), q.begin()));
  BOOST_CHECK(std::equal(q.begin(), q.end(), qtree.begin()));
  BOOST_CHECK(aterm_balanced_tree(q.begin(), q.size()) == qtree);

  int count = 0;

  std::for_each(qtree.begin(), qtree.end(), counter(count));

  BOOST_CHECK(count == 7);

  count = 0;

  std::for_each(qtree.begin(), qtree.end(), counter(count));

  aterm_balanced_tree qcopy(q.begin(),1);

  qtree.swap(qcopy);

  BOOST_CHECK(std::equal(qtree.begin(), qtree.end(), q.begin()));
  BOOST_CHECK(std::equal(q.begin(), q.end(), qtree.begin()));

  BOOST_CHECK(!std::equal(rtree.begin(), rtree.end(), q.begin()));
  BOOST_CHECK(!std::equal(q.begin(), q.end(), rtree.begin()));
}

BOOST_AUTO_TEST_CASE(test_empty_aterm_balanced_tree)
{
  aterm_balanced_tree empty_tree;
  BOOST_CHECK(empty_tree.begin() == empty_tree.end());

  aterm_list q = read_list_from_string("[]");
  aterm_list r(read_list_from_string("[]"));
  aterm_balanced_tree qtree(q.begin(),0);
  aterm_balanced_tree rtree(r.begin(),0);


  BOOST_CHECK(qtree.size() == 0);
  BOOST_CHECK(qtree.empty());

  for (int i = 0; i != 0; ++i)
  {
    BOOST_CHECK(qtree[i] == aterm_int(i));
  }

  BOOST_CHECK(std::equal(qtree.begin(), qtree.end(), q.begin()));
  BOOST_CHECK(std::equal(q.begin(), q.end(), qtree.begin()));
  BOOST_CHECK(aterm_balanced_tree(q.begin(), q.size()) == qtree);

  int count = 0;

  std::for_each(qtree.begin(), qtree.end(), counter(count));

  BOOST_CHECK(count == 0);

  count = 0;

  std::for_each(qtree.begin(), qtree.end(), counter(count));

  aterm_balanced_tree qcopy(q.begin(),0);

  qtree.swap(qcopy);

  BOOST_CHECK(std::equal(qtree.begin(), qtree.end(), q.begin()));
  BOOST_CHECK(std::equal(q.begin(), q.end(), qtree.begin()));

  BOOST_CHECK(std::equal(rtree.begin(), rtree.end(), q.begin()));
  BOOST_CHECK(std::equal(q.begin(), q.end(), rtree.begin()));
}


