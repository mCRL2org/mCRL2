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
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/invariant_checker.h"
#include "mcrl2/lps/invelm_algorithm.h"
#include "mcrl2/utilities/text_utility.h"
#include "test_specifications.h"

using namespace mcrl2;

inline
lps::specification invelm(const lps::specification& spec,
                          const data::data_expression& invariant,
                          bool simplify_all = false,
                          bool no_elimination = false
                         )
{
  lps::specification specification = spec;
  data::rewriter::strategy rewrite_strategy = data::jitty;
  int time_limit = 0;
  bool path_eliminator = false;
  data::detail::smt_solver_type solver_type = mcrl2::data::detail::solver_type_cvc;
  bool apply_induction = false;

  lps::detail::Invariant_Checker<lps::specification> v_invariant_checker(specification,
                                                     rewrite_strategy,
                                                     time_limit,
                                                     path_eliminator,
                                                     solver_type,
                                                     apply_induction,
                                                     simplify_all
                                                    );
  if (v_invariant_checker.check_invariant(invariant))
  {
    lps::invelm_algorithm<lps::specification> algorithm(specification,
                                    rewrite_strategy,
                                    time_limit,
                                    path_eliminator,
                                    solver_type,
                                    apply_induction,
                                    simplify_all
                                   );
    algorithm.run(invariant, !no_elimination);
  }
  return specification;
}

BOOST_AUTO_TEST_CASE(test_abp)
{
  lps::specification specification = lps::parse_linear_process_specification(LINEAR_ABP);
  data::data_expression invariant = data::parse_data_expression("true");

  lps::specification result = invelm(specification, invariant);
}

template <typename Expr>
bool has_identifier(const Expr& x, const std::string& id)
{
  std::set<core::identifier_string> ids = lps::find_identifiers(x);
  return ids.find(core::identifier_string(id)) != ids.end();
}

BOOST_AUTO_TEST_CASE(test_invariant)
{
  std::string SPEC =
    "act a, b, c, d;                         \n"
    "                                        \n"
    "proc P(b1, b2: Bool) =                  \n"
    "       b1 -> a . P(!b1, b2)             \n"
    "     + b2 -> b . P(true, b1 && b2)      \n"
    "     + (b1 && b2) -> c . P(false, false)\n"
    "     + d . P(false, true)               \n"
    "     + delta;                           \n"
    "                                        \n"
    "init P(false, true);                    \n"
    ;

  std::string INVARIANT = "!(b1 && b2)";

  data::data_expression invariant = data::parse_data_expression(INVARIANT, "b1, b2: Bool;");
  lps::specification spec = lps::parse_linear_process_specification(SPEC);
  bool simplify_all;
  bool no_elimination;
  lps::specification spec1;
  lps::linear_process proc;
  std::cout << lps::pp(spec) << std::endl;

  simplify_all = false;
  no_elimination = false;
  spec1 = invelm(spec, invariant, simplify_all, no_elimination);
  std::cout << lps::pp(spec1) << std::endl;
  proc = spec1.process();
  BOOST_CHECK(proc.action_summands().size() == 3);
  BOOST_CHECK(has_identifier(proc.action_summands()[2], "d"));
  BOOST_CHECK(proc.deadlock_summands().back().condition() == data::sort_bool::true_());

  simplify_all = true;
  no_elimination = false;
  spec1 = invelm(spec, invariant, simplify_all, no_elimination);
  std::cout << lps::pp(spec1) << std::endl;
  proc = spec1.process();
  BOOST_CHECK(proc.action_summands().size() == 3);
  BOOST_CHECK(has_identifier(proc.action_summands()[2], "d"));
  BOOST_CHECK(proc.deadlock_summands().back().condition() != invariant);
  BOOST_CHECK(proc.deadlock_summands().back().condition() != data::sort_bool::true_());
  BOOST_CHECK(has_identifier(proc.deadlock_summands().back().condition(), "b1"));

  simplify_all = true;
  no_elimination = true;
  spec1 = invelm(spec, invariant, simplify_all, no_elimination);
  std::cout << lps::pp(spec1) << std::endl;
  proc = spec1.process();
  BOOST_CHECK(proc.action_summands().size() == 4);
  BOOST_CHECK(has_identifier(proc.action_summands()[2], "c"));
  BOOST_CHECK(proc.deadlock_summands().back().condition() == invariant);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
