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

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/parser.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/map_substitution.h"
#include "mcrl2/data/sequence_substitution.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/assignment_list_substitution.h"
#include "mcrl2/core/garbage_collection.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::data;
using mcrl2::core::pp;

struct add_zero
{
  aterm_appl operator()(aterm_appl t) const
  {
    if (data_expression(t).is_variable())
    {
      variable d(t);
      return variable(std::string(d.name()) + "0", d.sort());
    }
    else
      return t;
  }
};

/// Function object for comparing a data variable with the
/// the left hand side of a data assignment.
struct compare_lhs
{
  variable m_variable;

  compare_lhs(const variable& variable)
    : m_variable(variable)
  {}

  bool operator()(const assignment& a) const
  {
    return m_variable == a.lhs();
  }
};

/// Utility class for applying a list of assignments to a term.
struct assignment_list_replacer
{
  const assignment_list& l;

  assignment_list_replacer(const assignment_list& l_)
    : l(l_)
  {}

  std::pair<aterm_appl, bool> operator()(aterm_appl t) const
  {
    if (!mcrl2::data::detail::is_variable(t))
    {
      return std::make_pair(t, true); // continue the recursion
    }
    assignment_list::const_iterator i = std::find_if(l.begin(), l.end(), compare_lhs(t));
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
  using namespace mcrl2::data::sort_bool;

  std::cerr << "replace" << std::endl;

  variable d("d", basic_sort("D"));
  variable e("e", basic_sort("D"));
  variable f("f", basic_sort("D"));
  variable d0("d0", basic_sort("D"));
  variable e0("e0", basic_sort("D"));
  variable f0("f0", basic_sort("D"));

  data_expression g = and_(equal_to(d, e), not_equal_to(e, f));

  data_expression h = replace(g, add_zero());
  BOOST_CHECK(h == and_(equal_to(d0, e0), not_equal_to(e0, f0)));

  data_expression i = replace(g, d, e);
  BOOST_CHECK(i == and_(equal_to(e, e), not_equal_to(e, f)));
}

void test_assignment_list()
{
  using namespace mcrl2::data::sort_bool;

  std::cerr << "assignment_list replace" << std::endl;

  variable d1("d1", basic_sort("D"));
  variable d2("d2", basic_sort("D"));
  variable d3("d3", basic_sort("D"));
  variable e1("e1", basic_sort("D"));
  variable e2("e2", basic_sort("D"));
  variable e3("e3", basic_sort("D"));

  assignment_vector l;
  l.push_back(assignment(d1, e1));
  l.push_back(assignment(e1, e2));
  l.push_back(assignment(e2, e3));

  data_expression t  = and_(equal_to(d1, e1), not_equal_to(e2, d3));
  data_expression t0 = and_(equal_to(e1, e2), not_equal_to(e3, d3));
  data_expression t1 = partial_replace(t, assignment_list_replacer(assignment_list(l.begin(), l.end())));
  data_expression t2 = assignment_list_substitution(assignment_list(l.begin(), l.end()))(t);
  std::cerr << "t  == " << mcrl2::core::pp(t) << std::endl;
  std::cerr << "t1 == " << mcrl2::core::pp(t1) << std::endl;
  std::cerr << "t2 == " << mcrl2::core::pp(t2) << std::endl;
  BOOST_CHECK(t0 == t1);
  BOOST_CHECK(t0 == t2);

  assignment_list m1 = atermpp::make_list(
    assignment(d1, d2),
    assignment(e1, d1)
  );
  assignment_list m2 = data::replace_variables(m1, assignment(d2, d1));
  assignment_list m3 = atermpp::make_list(
    assignment(d1, d1),
    assignment(e1, d1)
  );
  BOOST_CHECK(m2 == m3);
  std::cout << "<m2>" << mcrl2::core::pp(m2) << std::endl;
  std::cout << "<m3>" << mcrl2::core::pp(m3) << std::endl;
}

void test_variable_replace()
{
  using namespace mcrl2::data::sort_bool;

  std::cerr << "variable replace" << std::endl;

  variable d1("d1", basic_sort("D"));
  variable d2("d2", basic_sort("D"));
  variable d3("d3", basic_sort("D"));
  variable_vector variables;
  variables.push_back(d1);
  variables.push_back(d2);
  variables.push_back(d3);

  variable x("x", basic_sort("D"));
  variable y("y", basic_sort("D"));
  variable z("z", basic_sort("D"));
  data_expression e1 = x;
  data_expression e2 = z;
  data_expression e3 = y;
  data_expression_vector replacements;
  replacements.push_back(e1);
  replacements.push_back(e2);
  replacements.push_back(e3);

  std::vector<variable> v;
  v.push_back(d1);
  v.push_back(d2);
  v.push_back(d3);
  std::list<data_expression> l;
  l.push_back(e1);
  l.push_back(e2);
  l.push_back(e3);

  data_expression t  = and_(equal_to(d1, d2), not_equal_to(d2, d3));
  data_expression t1 = make_double_sequence_substitution_adaptor(variables, replacements)(t);
  data_expression t2 = make_double_sequence_substitution_adaptor(v, l)(t);
  std::cerr << "t  == " << mcrl2::core::pp(t) << std::endl;
  std::cerr << "t1 == " << mcrl2::core::pp(t1) << std::endl;
  std::cerr << "t2 == " << mcrl2::core::pp(t2) << std::endl;
  BOOST_CHECK(t1 == t2);

  t = and_(equal_to(d1, d2), not_equal_to(d2, d3));
  BOOST_CHECK(t1 == replace_variables(t, make_double_sequence_substitution_adaptor(variables, replacements)));
  BOOST_CHECK(t1 == replace_variables(t, make_double_sequence_substitution(variables, replacements)));
  BOOST_CHECK(t1 == replace_variables(t, make_double_sequence_substitution(v, l)));
  BOOST_CHECK(t1 == replace_variables(t, make_map_substitution(variables, replacements)));
  BOOST_CHECK(t1 == make_double_sequence_substitution_adaptor(variables, replacements)(t));
  BOOST_CHECK(t1 == make_double_sequence_substitution(variables, replacements)(t));
  BOOST_CHECK(t1 == make_double_sequence_substitution(v, l)(t));
  BOOST_CHECK(t1 == make_map_substitution(variables, replacements)(t));
}

void test_replace_with_binders()
{
  std::cerr << "replace with binders" << std::endl;
  mutable_map_substitution< > sigma;
  data_expression                                   input1(variable("c", sort_bool::bool_()));
  data_expression                                   input2(parse_data_expression("exists b: Bool, c: Bool. if(b, c, b)"));

  sigma[variable("c", sort_bool::bool_())] = sort_bool::false_();

  BOOST_CHECK(replace_free_variables(input1, sigma) == sort_bool::false_());

  // variable c is bound and should not be replaced
  BOOST_CHECK(replace_free_variables(input2, sigma) == input2);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_replace();
  core::garbage_collect();

  test_assignment_list();
  core::garbage_collect();

  test_variable_replace();
  core::garbage_collect();

  test_replace_with_binders();
  core::garbage_collect();

  return 0;
}
