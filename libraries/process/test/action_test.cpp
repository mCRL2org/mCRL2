// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file action_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE action_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/atermpp/aterm_structural_order.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/process/print.h"
#include "mcrl2/process/process_expression.h"

using namespace mcrl2;

BOOST_AUTO_TEST_CASE(test_main)
{
  data::sort_expression X(data::basic_sort("X"));
  data::sort_expression Y(data::basic_sort("Y"));

  process::action_label aX(core::identifier_string("a"),  { X });
  process::action_label aY(core::identifier_string("a"),  { Y });
  process::action_label aXY(core::identifier_string("a"), { X, Y });
  process::action_label bX(core::identifier_string("b"),  { X });

  data::data_expression x_X = data::variable("x", X);
  data::data_expression y_X = data::variable("y", X);
  data::data_expression y_Y = data::variable("y", Y);

  process::action a1(aX, { x_X });
  process::action a2(aX, { y_X });
  process::action a3(bX, { x_X });

  BOOST_CHECK(process::equal_signatures(a1, a1));
  BOOST_CHECK(process::equal_signatures(a1, a2));
  BOOST_CHECK(!process::equal_signatures(a1, a3));

  process::action_label_list l;
  l.push_front(aX);
  BOOST_CHECK(l.front()==aX);
  l.push_front(aY);
  l.push_front(aY);
  BOOST_CHECK(l.front()==aY);
  process::action_label_list l1=l;
  BOOST_CHECK(l1.front()==aY);
  BOOST_CHECK(l.front()==aY);
  process::action_label a=l.front();
  BOOST_CHECK(a==aY);
}

// The documented order on action labels and actions: name (string order), then sorts, then arguments. Sorts and
// arguments are compared structurally.
static bool documented_less(const process::action_label& a1, const process::action_label& a2)
{
  const std::string s1 = a1.name();
  const std::string s2 = a2.name();
  if (s1 != s2)
  {
    return s1 < s2;
  }
  return atermpp::structural_less()(a1.sorts(), a2.sorts());
}

static bool documented_less(const process::action& a1, const process::action& a2)
{
  if (a1.label() != a2.label())
  {
    return documented_less(a1.label(), a2.label());
  }
  return atermpp::structural_less()(a1.arguments(), a2.arguments());
}

// operator< on actions and action labels is defined as the structural order on terms. Check that this coincides
// with the documented order, and that action_name_compare coincides with string comparison.
BOOST_AUTO_TEST_CASE(test_order)
{
  const data::sort_expression X(data::basic_sort("X"));
  const data::sort_expression Y(data::basic_sort("Y"));
  const data::sort_expression N(data::sort_nat::nat());

  // Names that are prefixes of each other, differ in case, or contain digits.
  const std::vector<std::string> names = {"a", "ab", "b", "B", "a1", "a10", "a2", "_a"};
  const std::vector<data::sort_expression_list> sorts = {{}, {X}, {Y}, {X, Y}, {Y, X}, {N}};

  std::vector<process::action_label> labels;
  for (const std::string& name: names)
  {
    for (const data::sort_expression_list& s: sorts)
    {
      labels.emplace_back(core::identifier_string(name), s);
    }
  }

  // Equal labels with different arguments, including arguments of different kinds.
  const data::variable x("x", X);
  const data::variable y("y", X);
  const data::variable z("z", Y);
  const data::variable n("n", N);
  std::vector<process::action> actions;
  for (const process::action_label& l: labels)
  {
    if (l.sorts() == data::sort_expression_list({X}))
    {
      actions.emplace_back(l, data::data_expression_list({x}));
      actions.emplace_back(l, data::data_expression_list({y}));
    }
    else if (l.sorts() == data::sort_expression_list({X, Y}))
    {
      actions.emplace_back(l, data::data_expression_list({x, z}));
      actions.emplace_back(l, data::data_expression_list({y, z}));
    }
    else if (l.sorts() == data::sort_expression_list({N}))
    {
      actions.emplace_back(l, data::data_expression_list({n}));
      actions.emplace_back(l, data::data_expression_list({data::sort_nat::nat(0)}));
      actions.emplace_back(l, data::data_expression_list({data::sort_nat::nat(1)}));
      actions.emplace_back(l, data::data_expression_list({data::sort_nat::nat(10)}));
    }
    else if (l.sorts().empty())
    {
      actions.emplace_back(l, data::data_expression_list());
    }
  }

  for (const process::action_label& l1: labels)
  {
    for (const process::action_label& l2: labels)
    {
      BOOST_CHECK_MESSAGE((l1 < l2) == documented_less(l1, l2), "labels " << process::pp(l1) << " and " << process::pp(l2));
      BOOST_CHECK_EQUAL(l1 < l2, atermpp::structural_less()(l1, l2));
      BOOST_CHECK_EQUAL(process::action_name_compare()(l1.name(), l2.name()),
                        std::string(l1.name()) < std::string(l2.name()));
    }
  }

  for (const process::action& a1: actions)
  {
    for (const process::action& a2: actions)
    {
      BOOST_CHECK_MESSAGE((a1 < a2) == documented_less(a1, a2), "actions " << process::pp(a1) << " and " << process::pp(a2));
      BOOST_CHECK_EQUAL(a1 < a2, atermpp::structural_less()(a1, a2));
      // The order on actions is consistent with the order on their labels.
      if (a1.label() < a2.label())
      {
        BOOST_CHECK(a1 < a2);
      }
    }
  }
}
