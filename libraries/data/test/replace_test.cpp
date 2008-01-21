// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file replace_test.cpp
/// \brief Add your file description here.

// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : test/algorithm.cpp
// date          : 19/09/06
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#include <iostream>
#include <iterator>
#include <list>
#include <vector>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_variable_replace.h"
#include "mcrl2/utilities/aterm_ext.h"

using namespace atermpp;
using namespace mcrl2::data;

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
  std::cerr << "t  == " << pp(t) << std::endl;
  std::cerr << "t1 == " << pp(t1) << std::endl;
  std::cerr << "t2 == " << pp(t2) << std::endl;
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

  data_variable x("x:X");
  data_variable y("y:X");
  data_variable z("z:X");
  data_expression e1 = equal_to(x, y);
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
  data_expression t1 = replace_data_variable_sequence(t, variables, replacements);
  data_expression t2 = replace_data_variable_sequence(t, v, l);
  std::cerr << "t  == " << pp(t) << std::endl;
  std::cerr << "t1 == " << pp(t1) << std::endl;
  std::cerr << "t2 == " << pp(t2) << std::endl;
  BOOST_CHECK(t1 == t2);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  test_replace();
  test_data_assignment_list();
  test_data_variable_replace();

  return 0;
}
