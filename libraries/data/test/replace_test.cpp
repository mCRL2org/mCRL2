// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file replace_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <vector>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/replace.h"

using namespace atermpp;
using namespace mcrl2::data;
using mcrl2::core::pp;

struct add_zero
{
  aterm_appl operator()(aterm_appl t) const
  {
    if (is_data_variable(t))
    {
      data_variable d(t);
      return data_variable(std::string(d.name()) + "0", d.sort());
    }
    else
      return t;
  }
};

/// Function object for comparing a data variable with the
/// the left hand side of a data assignment.
struct compare_lhs
{
  data_variable m_variable;

  compare_lhs(const data_variable& variable)
    : m_variable(variable)
  {}

  bool operator()(const data_assignment& a) const
  {
    return m_variable == a.lhs();
  }
};

/// Utility class for applying a list of assignments to a term.
struct data_assignment_list_replacer
{
  const data_assignment_list& l;

  data_assignment_list_replacer(const data_assignment_list& l_)
    : l(l_)
  {}

  std::pair<aterm_appl, bool> operator()(aterm_appl t) const
  {
    if (!is_data_variable(t))
    {
      return std::make_pair(t, true); // continue the recursion
    }
    data_assignment_list::iterator i = std::find_if(l.begin(), l.end(), compare_lhs(t));
    if (i == l.end())
    {
      return std::make_pair(t, false); // don't continue the recursion
    }
    else
    {
      return std::make_pair(i->rhs(), false); // don't continue the recursion
    }
  }
};

void test_replace()
{
  using namespace mcrl2::data::data_expr;

  data_variable d("d:D");
  data_variable e("e:D");
  data_variable f("f:D");
  data_variable d0("d0:D");
  data_variable e0("e0:D");
  data_variable f0("f0:D");

  data_expression g = and_(equal_to(d, e), not_equal_to(e, f));

  data_expression h = replace(g, add_zero());
  BOOST_CHECK(h == and_(equal_to(d0, e0), not_equal_to(e0, f0)));

  data_expression i = replace(g, d, e);
  BOOST_CHECK(i == and_(equal_to(e, e), not_equal_to(e, f)));
}

void test_data_assignment_list()
{
  using namespace mcrl2::data::data_expr;

  data_variable d1("d1:D");
  data_variable d2("d2:D");
  data_variable d3("d3:D");
  data_variable e1("e1:D");
  data_variable e2("e2:D");
  data_variable e3("e3:D");

  data_assignment_list l;
  l = push_front(l, data_assignment(d1, e1));
  l = push_front(l, data_assignment(e1, e2));
  l = push_front(l, data_assignment(e2, e3));

  data_expression t  = and_(equal_to(d1, e1), not_equal_to(e2, d3));
  data_expression t0 = and_(equal_to(e1, e2), not_equal_to(e3, d3));
  data_expression t1 = partial_replace(t, data_assignment_list_replacer(l));
  data_expression t2 = t.substitute(assignment_list_substitution(l));
  std::cerr << "t  == " << mcrl2::core::pp(t) << std::endl;
  std::cerr << "t1 == " << mcrl2::core::pp(t1) << std::endl;
  std::cerr << "t2 == " << mcrl2::core::pp(t2) << std::endl;
  BOOST_CHECK(t0 == t1);
  BOOST_CHECK(t0 == t2);
}

void test_data_variable_replace()
{
  using namespace mcrl2::data::data_expr;

  data_variable d1("d1:D");
  data_variable d2("d2:D");
  data_variable d3("d3:D");
  data_variable_list variables;
  variables = push_front(variables, d1);
  variables = push_front(variables, d2);
  variables = push_front(variables, d3);

  data_variable x("x:D");
  data_variable y("y:D");
  data_variable z("z:D");
  data_expression e1 = x;
  data_expression e2 = z;
  data_expression e3 = y;
  data_expression_list replacements;
  replacements = push_front(replacements, e1);
  replacements = push_front(replacements, e2);
  replacements = push_front(replacements, e3);

  std::vector<data_variable> v;
  v.push_back(d1);
  v.push_back(d2);
  v.push_back(d3);
  std::list<data_expression> l;
  l.push_back(e1);
  l.push_back(e2);
  l.push_back(e3);

  data_expression t  = and_(equal_to(d1, d2), not_equal_to(d2, d3));
  data_expression t1 = data_variable_sequence_replace(t, variables, replacements);
  data_expression t2 = data_variable_sequence_replace(t, v, l);
  std::cerr << "t  == " << mcrl2::core::pp(t) << std::endl;
  std::cerr << "t1 == " << mcrl2::core::pp(t1) << std::endl;
  std::cerr << "t2 == " << mcrl2::core::pp(t2) << std::endl;
  BOOST_CHECK(t1 == t2);
}

void test_data_expression_replace()
{
  // y:Real
  data_variable y("y",sort_expr::real());
  data_expression e(y);
  // 4:Real
  data_expression x(data_expr::real(4));
  // [y]
  data_expression_list el;
  el = push_front(el, e);
  // [4]
  data_expression_list xl;
  xl = push_front(xl, x);

  // y := 4
  std::map<data_expression, data_expression> replacements;
  replacements[e] = x;

  std::cerr << e << std::endl;
  data_expression e_ = data_expression_map_replace(e, replacements);
  std::cerr << e_ << std::endl;
  BOOST_CHECK(e_ == x);

  std::cerr << xl << std::endl;
  data_expression_list xl_ = data_expression_map_replace(el, replacements);
  std::cerr << xl_ << std::endl;
  BOOST_CHECK(xl_ == xl);

  data_expression u = data_expr::plus(data_expr::real(4), data_expr::real(1));
  data_expression v = data_expr::plus(y, data_expr::real(1));
  std::cerr << "u = " << mcrl2::core::pp(u) << std::endl;
  std::cerr << "v = " << mcrl2::core::pp(v) << std::endl;
  data_expression v_ = data_expression_map_replace(v, replacements);
  std::cerr << "v_ = " << mcrl2::core::pp(v_) << std::endl;
  BOOST_CHECK(v_ != u);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_replace();
  test_data_assignment_list();
  test_data_variable_replace();
  test_data_expression_replace();

  return 0;
}
