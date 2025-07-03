// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file multi_action_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE multi_action_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/consistency.h"
#include "mcrl2/data/detail/test_rewriters.h"
#include "mcrl2/lps/print.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

/// \brief Returns a data variable of type Nat with a given name
/// \param name A string
/// \return A data variable of type Nat with a given name
data::variable nat(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_nat::nat());
}

process::action act(const std::string& name, data_expression_list parameters)
{
  std::vector<sort_expression> sorts;
  for (const auto & parameter : parameters)
  {
    sorts.push_back(parameter.sort());
  }
  process::action_label label(name, sort_expression_list(sorts.begin(), sorts.end()));
  return process::action(label, parameters);
}

void test_multi_actions(const process::action_list& a, const process::action_list& b, const data_expression& expected_result = data::undefined_data_expression())
{
  std::cout << "--- test_multi_actions ---" << std::endl;
  data_expression result = equal_multi_actions(multi_action(a), multi_action(b));
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
  process::action_list a1  ( { act("a", data_expression_list({ d1 })) });
  process::action_list a2  ( { act("a", data_expression_list({ d2 })) });
  process::action_list b1  ( { act("b", data_expression_list({ d1 })) });
  process::action_list b2  ( { act("b", data_expression_list({ d2 })) });
  process::action_list a11 ( { act("a", data_expression_list({ d1 })), act("a", data_expression_list({ d1 })) });
  process::action_list a12 ( { act("a", data_expression_list({ d1 })), act("a", data_expression_list({ d2 })) });
  process::action_list a21 ( { act("a", data_expression_list({ d2 })), act("a", data_expression_list({ d1 })) });
  process::action_list a22 ( { act("a", data_expression_list({ d2 })), act("a", data_expression_list({ d2 })) });
  process::action_list a34 ( { act("a", data_expression_list({ d3 })), act("a", data_expression_list({ d4 })) });
  process::action_list a12b1 ( { act("a", data_expression_list({ d1 })), act("a", data_expression_list({ d2 })), act("b", data_expression_list({ d1 })) });
  process::action_list a34b2 ( { act("a", data_expression_list({ d3 })), act("a", data_expression_list({ d4 })), act("b", data_expression_list({ d2 })) });
  test_multi_actions(a1,  a1, d::true_());
  test_multi_actions(a1,  a2, d::equal_to(d1, d2));
  test_multi_actions(a11, a11, d::true_());
  test_multi_actions(a12, a21, d::true_());
  test_multi_actions(a21, a12, d::true_());
  test_multi_actions(a11, a22, d::equal_to(d1, d2));
  test_multi_actions(a1, a12,  d::false_());
  test_multi_actions(a1, b1,   d::false_());
  test_multi_actions(a12, a34);
  test_multi_actions(a12b1, a34b2);

  data_expression m1 = equal_multi_actions(multi_action(a12), multi_action(a34));
  data_expression m2 = equal_multi_actions(multi_action(a34), multi_action(a12));
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
  process::action_list a1  ( { act("a", data_expression_list({ d1 })) });
  process::action_list a2  ( { act("a", data_expression_list({ d2 })) });
  process::action_list b1  ( { act("b", data_expression_list({ d1 })) });
  process::action_list a11 ( { act("a", data_expression_list({ d1 })), act("a", data_expression_list({ d1 })) });
  multi_action m(a11);
  std::string s = lps::pp(m);
  std::cout << "s = " << s << std::endl;
  BOOST_CHECK(s == "a(d1)|a(d1)");
}

BOOST_AUTO_TEST_CASE(test_main)
{
  test_equal_multi_actions();
  test_pp();
}
