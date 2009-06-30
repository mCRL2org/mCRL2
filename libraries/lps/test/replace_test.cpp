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
#include "mcrl2/data/parser.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/substitute.h"
#include "mcrl2/lps/detail/lps_replacer.h"
#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/core/garbage_collection.h"

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
  summand s = spec.process().summands().front();
  variable b("b", sort_bool::bool_());
  variable c("c", sort_bool::bool_());
  variable d("d", sort_bool::bool_());
  assignment a(c, d);
  summand t = data::replace_variables(s, a); // must become lps::replace
  std::cout << "<s>" << pp(s) << std::endl;
  std::cout << "<t>" << pp(t) << std::endl;
  core::garbage_collect();
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

void test_lps_replacer()
{
  specification spec1 = parse_linear_process_specification(SPEC1a);
  specification spec2 = parse_linear_process_specification(SPEC1b);
  data::mutable_map_substitution<> sigma;
  sigma[variable("s", sort_pos::pos())] = sort_pos::pos(3);
  sigma[variable("i", sort_nat::nat())] = sort_nat::nat(4);
  lps::detail::lps_replacer<data::mutable_map_substitution<> > subst(sigma);

  data::data_expression d;
  subst(d);
  data::assignment a;
  subst(a);

  subst(spec1);
  std::cerr << pp(spec1.process()) << std::endl;
  std::cerr << "-------------------------------------" << std::endl;
  std::cerr << pp(spec2.process()) << std::endl;
  BOOST_CHECK(pp(spec1.process()) == pp(spec2.process()));
  core::garbage_collect();
}

void test_lps_substitute()
{
  data::variable v("v", sort_pos::pos());
  data::variable w("w", sort_pos::pos());
  data::mutable_map_substitution<> sigma;
  sigma[v] = w;

  lps::substitute(v, sigma);
  core::garbage_collect();
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_replace();
  test_lps_replacer();
  test_lps_substitute();

  return 0;
}
