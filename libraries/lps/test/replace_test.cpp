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
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/replace.h"
#include "mcrl2/lps/detail/specification_property_map.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

const std::string SPEC =
  "act  a;                  \n"
  "                         \n"
  "proc P(b: Bool) =        \n"
  "       sum c: Bool.      \n"
  "         (b == c) ->     \n"
  "         a .             \n"
  "         P(c);           \n"
  "                         \n"
  "init P(true);            \n"
  ;

void test_replace()
{
  specification spec = parse_linear_process_specification(SPEC);
  action_summand s = spec.process().action_summands().front();
  variable b("b", sort_bool::bool_());
  variable c("c", sort_bool::bool_());
  variable d("d", sort_bool::bool_());
  assignment a(c, d);
  action_summand t = s;
  lps::replace_variables(t, a);
}

std::string SPEC1a =
  "act  action: Nat;         \n"
  "                          \n"
  "proc P(s: Pos, i: Nat) =  \n"
  "       (s == 2) ->        \n"
  "         action(i) .      \n"
  "         P(1, i)          \n"
  "     + (s == 1) ->        \n"
  "         action(i) .      \n"
  "         P(2, i)          \n"
  "     + true ->            \n"
  "         delta;           \n"
  "                          \n"
  "init P(1, 0);             \n"
  ;

std::string SPEC1b =
  "act  action: Nat;         \n"
  "                          \n"
  "proc P(s: Pos, i: Nat) =  \n"
  "       (3 == 2) ->        \n"
  "         action(4) .      \n"
  "         P(1, 4)          \n"
  "     + (3 == 1) ->        \n"
  "         action(4) .      \n"
  "         P(2, 4)          \n"
  "     + true ->            \n"
  "         delta;           \n"
  "                          \n"
  "init P(1, 0);             \n"
  ;

void test_lps_substituter()
{
  specification spec1 = parse_linear_process_specification(SPEC1a);
  specification spec2 = parse_linear_process_specification(SPEC1b);
  data::mutable_map_substitution<> sigma;
  sigma[variable("s", sort_pos::pos())] = sort_pos::pos(3);
  sigma[variable("i", sort_nat::nat())] = sort_nat::nat(4);

  lps::replace_variables(spec1, sigma);
  std::cerr << lps::pp(spec1.process()) << std::endl;
  std::cerr << "-------------------------------------" << std::endl;
  std::cerr << lps::pp(spec2.process()) << std::endl;
  BOOST_CHECK(lps::pp(spec1.process()) == lps::pp(spec2.process()));
}

void test_lps_substitute()
{
  data::variable v("v", sort_pos::pos());
  data::variable w("w", sort_pos::pos());
  data::mutable_map_substitution<> sigma;
  sigma[v] = w;
  lps::replace_free_variables(v, sigma);
}

void test_replace_process_parameters()
{
  std::string SPEC =
    "act a;                        \n"
    "proc P(b:Bool) = a.P(b = !b); \n"
    "init P(true);                 \n"
    ;
  specification spec = parse_linear_process_specification(SPEC);
  data::mutable_map_substitution<> sigma;
  data::variable b("b", data::sort_bool::bool_());
  data::variable b0("b0", data::sort_bool::bool_());
  sigma[b] = b0;
  lps::replace_process_parameters(spec, sigma);
  std::set<data::variable> variables = lps::find_variables(spec);
  BOOST_CHECK(variables.find(b) == variables.end());
}

void test_replace_summand_variables()
{
  std::string SPEC =
    "act a;                                   \n"
    "proc P(b:Bool) = sum c:Bool. a.P(b = c); \n"
    "init P(true);                            \n"
    ;
  specification spec = parse_linear_process_specification(SPEC);
  data::mutable_map_substitution<> sigma;
  data::variable c("c", data::sort_bool::bool_());
  data::variable c0("c0", data::sort_bool::bool_());
  sigma[c] = c0;
  lps::replace_summand_variables(spec, sigma);
  std::cout << lps::pp(spec) << std::endl;
  std::set<data::variable> variables = lps::find_variables(spec);
  BOOST_CHECK(variables.find(c) == variables.end());
}

void test_action_list()
{
  sort_expression_list s;
  s.push_front(sort_expression(sort_nat::nat()));
  action_label label(core::identifier_string("a"), s);

  variable b("b", data::sort_bool::bool_());
  variable c("c", data::sort_bool::bool_());
  data_expression_list e1;
  e1.push_front(data_expression(sort_bool::and_(b, c)));
  data_expression_list e2;
  e2.push_front(data_expression(sort_bool::and_(c, c)));

  action_list l1;
  action a1(label, e1);
  l1.push_front(a1);

  action_list l2;
  action a2(label, e2);
  l2.push_front(a2);

  data::mutable_map_substitution<> sigma;
  sigma[b] = c;

  l1 = lps::replace_free_variables(l1, sigma);
  BOOST_CHECK(l1 == l2);
}

int test_main(int argc, char* argv[])
{
  test_replace();
  test_lps_substituter();
  test_lps_substitute();
  test_replace_process_parameters();
  test_replace_summand_variables();
  test_action_list();

  return 0;
}
