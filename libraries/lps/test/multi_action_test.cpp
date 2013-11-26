// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parelm_test.cpp
/// \brief Add your file description here.

//#define MCRL2_LPS_PARELM_DEBUG

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/detail/test_rewriters.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lps/print.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

/// \brief Returns a data variable of type Nat with a given name
/// \param name A string
/// \return A data variable of type Nat with a given name
data::variable nat(std::string name)
{
  return data::variable(core::identifier_string(name), data::sort_nat::nat());
}

action act(std::string name, data_expression_list parameters)
{
  std::vector<sort_expression> sorts;
  for (data_expression_list::iterator i = parameters.begin(); i != parameters.end(); ++i)
  {
    sorts.push_back(i->sort());
  }
  action_label label(name, sort_expression_list(sorts.begin(), sorts.end()));
  return action(label, parameters);
}

void test_multi_actions(action_list a, action_list b, data_expression expected_result = data::undefined_data_expression())
{
  std::cout << "--- test_multi_actions ---" << std::endl;
  data_expression result = equal_multi_actions(a, b);
  std::cout << "a               = " << lps::pp(a) << std::endl;
  std::cout << "b               = " << lps::pp(b) << std::endl;
  std::cout << "result          = " << lps::pp(result) << std::endl;
  std::cout << "expected_result = " << lps::pp(expected_result) << std::endl;
  BOOST_CHECK(expected_result == data::undefined_data_expression() || result == expected_result);
}

void test_equal_multi_actions()
{
  namespace d = data;

  data_expression d1 = nat("d1");
  data_expression d2 = nat("d2");
  data_expression d3 = nat("d3");
  data_expression d4 = nat("d4");
  action_list a1  = make_list(act("a", make_list(d1)));
  action_list a2  = make_list(act("a", make_list(d2)));
  action_list b1  = make_list(act("b", make_list(d1)));
  action_list b2  = make_list(act("b", make_list(d2)));
  action_list a11 = make_list(act("a", make_list(d1)), act("a", make_list(d1)));
  action_list a12 = make_list(act("a", make_list(d1)), act("a", make_list(d2)));
  action_list a21 = make_list(act("a", make_list(d2)), act("a", make_list(d1)));
  action_list a22 = make_list(act("a", make_list(d2)), act("a", make_list(d2)));
  action_list a34 = make_list(act("a", make_list(d3)), act("a", make_list(d4)));
  action_list a12b1 = make_list(act("a", make_list(d1)), act("a", make_list(d2)), act("b", make_list(d1)));
  action_list a34b2 = make_list(act("a", make_list(d3)), act("a", make_list(d4)), act("b", make_list(d2)));
  test_multi_actions(a1,  a1, d::sort_bool::true_());
  test_multi_actions(a1,  a2, d::equal_to(d1, d2));
  test_multi_actions(a11, a11, d::sort_bool::true_());
  test_multi_actions(a12, a21, d::sort_bool::true_());
  test_multi_actions(a21, a12, d::sort_bool::true_());
  test_multi_actions(a11, a22, d::equal_to(d1, d2));
  test_multi_actions(a1, a12,  d::sort_bool::false_());
  test_multi_actions(a1, b1,   d::sort_bool::false_());
  test_multi_actions(a12, a34);
  test_multi_actions(a12b1, a34b2);

  data_expression m1 = equal_multi_actions(a12, a34);
  data_expression m2 = equal_multi_actions(a34, a12);
  data_expression n1 = data::detail::normalize_equality(m1);
  data_expression n2 = data::detail::normalize_equality(m2);
  data_expression p1 = data::detail::normalize_and_or(n1);
  data_expression p2 = data::detail::normalize_and_or(n2);
  std::cout << "m1 = " << data::pp(m1) << std::endl;
  std::cout << "m2 = " << data::pp(m2) << std::endl;
  std::cout << "n1 = " << data::pp(n1) << std::endl;
  std::cout << "n2 = " << data::pp(n2) << std::endl;
  std::cout << "p1 = " << data::pp(p1) << std::endl;
  std::cout << "p2 = " << data::pp(p2) << std::endl;
  BOOST_CHECK(p1 == p2);
}

void test_pp()
{
  data_expression d1 = nat("d1");
  data_expression d2 = nat("d2");
  action_list a1  = make_list(act("a", make_list(d1)));
  action_list a2  = make_list(act("a", make_list(d2)));
  action_list b1  = make_list(act("b", make_list(d1)));
  action_list a11 = make_list(act("a", make_list(d1)), act("a", make_list(d1)));
  multi_action m(a11);
  std::string s = lps::pp(m);
  std::cout << "s = " << s << std::endl;
  BOOST_CHECK(s == "a(d1)|a(d1)");
}

int test_main(int argc, char* argv[])
{
  test_equal_multi_actions();
  test_pp();

  return 0;
}
