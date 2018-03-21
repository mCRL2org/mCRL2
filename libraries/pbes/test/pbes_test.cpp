// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE pbes_test
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/complement.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/is_bes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

BOOST_AUTO_TEST_CASE(test_pbes)
{
  const std::string SPECIFICATION =
    "act a:Nat;                               \n"
    "                                         \n"
    "map smaller: Nat#Nat -> Bool;            \n"
    "                                         \n"
    "var x,y : Nat;                           \n"
    "                                         \n"
    "eqn smaller(x,y) = x < y;                \n"
    "                                         \n"
    "proc P(n:Nat) = sum m: Nat. a(m). P(m);  \n"
    "                                         \n"
    "init P(0);                               \n";

  const std::string FORMULA2 = "forall m:Nat. [a(m)]false";

  lps::specification spec = lps::remove_stochastic_operators(lps::linearise(SPECIFICATION));
  state_formulas::state_formula formula = state_formulas::parse_state_formula(FORMULA2, spec);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  pbes_expression e = p.equations().front().formula();

  BOOST_CHECK(!is_bes(p));
  BOOST_CHECK(!is_bes(e));

  try
  {
    load_pbes(p, "non-existing file");
    BOOST_CHECK(false); // loading is expected to fail
  }
  catch (mcrl2::runtime_error e)
  {
  }

  std::string filename = "write_term_to_text_stream.pbes";
  try
  {
    atermpp::aterm t = atermpp::read_term_from_string("f(x)");
    std::ofstream os;
    os.open(filename.c_str());
    atermpp::write_term_to_text_stream(t, os);
    os.close();
    load_pbes(p, filename);
    BOOST_CHECK(false); // loading is expected to fail
  }
  catch (mcrl2::runtime_error e)
  {
    remove(filename.c_str());
  }
  filename = "pbes_test_file.pbes";
  save_pbes(p, filename);
  load_pbes(p, filename);
  remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_global_variables)
{
  std::string TEXT =
    "glob k, m, n:Nat;                        \n"
    "                                         \n"
    "pbes                                     \n"
    "   mu X1(n1, m1:Nat) = X2(n1) || X2(m1); \n"
    "   mu X2(n2:Nat)     = X1(n2, n);        \n"
    "                                         \n"
    "init                                     \n"
    "   X1(m, n);                             \n"
    ;

  pbes p;
  std::stringstream s(TEXT);
  s >> p;
  std::set<data::variable> freevars = p.global_variables();
  BOOST_CHECK(freevars.size() == 3);  // The global variable k does not occur in the specification,
  // but occurs in the global variables list.
}

BOOST_AUTO_TEST_CASE(test_complement_method_builder)
{
  using namespace pbes_system;

  data::variable X("x", data::bool_());
  data::variable Y("y", data::bool_());

  pbes_expression p = or_(and_(X,Y), and_(Y,X));
  pbes_expression q = and_(or_(data::not_(X), data::not_(Y)), or_(data::not_(Y), data::not_(X)));
  std::cout << "p             = " << mcrl2::pbes_system::pp(p) << std::endl;
  std::cout << "q             = " << mcrl2::pbes_system::pp(q) << std::endl;
  std::cout << "complement(p) = " << mcrl2::pbes_system::pp(complement(p)) << std::endl;
  BOOST_CHECK(complement(p) == q);
}

BOOST_AUTO_TEST_CASE(test_pbes_expression)
{
  data::variable x1("x1", data::basic_sort("X"));
  pbes_expression e = x1;
}

BOOST_AUTO_TEST_CASE(test_trivial)
{
  const std::string TRIVIAL_FORMULA  = "[true*]<true*>true";
  lps::specification spec = lps::remove_stochastic_operators(lps::linearise(lps::detail::ABP_SPECIFICATION()));
  state_formulas::state_formula formula = state_formulas::parse_state_formula(TRIVIAL_FORMULA, spec);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());
}

BOOST_AUTO_TEST_CASE(test_instantiate_global_variables)
{
  std::string spec_text =
    "act a,b:Nat;             \n"
    "    d;                   \n"
    "proc P(n:Nat)=a(n).delta;\n"
    "init d.P(1);             \n"
    ;
  std::string formula_text = "([true*.a(1)]  (mu X.([!a(1)]X && <true> true)))";
  lps::specification spec = lps::remove_stochastic_operators(lps::linearise(spec_text));
  state_formulas::state_formula formula = state_formulas::parse_state_formula(formula_text, spec);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  std::cout << "<before>" << mcrl2::pbes_system::pp(p) << std::endl;
  std::cout << "<lps>" << lps::pp(spec) << std::endl;
  pbes_system::detail::instantiate_global_variables(p);
  std::cout << "<after>" << pbes_system::pp(p) << std::endl;
}

BOOST_AUTO_TEST_CASE(test_find_sort_expressions)
{
  const std::string TRIVIAL_FORMULA  = "[true*]<true*>true";
  lps::specification spec = lps::remove_stochastic_operators(lps::linearise(lps::detail::ABP_SPECIFICATION()));
  state_formulas::state_formula formula = state_formulas::parse_state_formula(TRIVIAL_FORMULA, spec);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  std::set<data::sort_expression> s;
  pbes_system::find_sort_expressions(p, std::inserter(s, s.end()));
  std::cout << core::detail::print_set(s) << std::endl;
}

BOOST_AUTO_TEST_CASE(test_io)
{
  using namespace pbes_system;

  std::string PBES_SPEC =
    "pbes nu X1 = X2; \n"
    "     mu X2 = X2; \n"
    "                 \n"
    "init X1;         \n"
    ;
  pbes p = txt2pbes(PBES_SPEC);
  save_pbes(p, "pbes.pbes",  pbes_format_internal());
  load_pbes(p, "pbes.pbes",  pbes_format_internal());
  save_pbes(p, "pbes.txt",   pbes_format_text());
  load_pbes(p, "pbes.txt",   pbes_format_text());
}

BOOST_AUTO_TEST_CASE(test_is_bes)
{
  // found with random testing 14-1-2011
  using namespace pbes_system;
  std::string text =
    "pbes nu X =       \n"
    "       val(true); \n"
    "                  \n"
    "init X;           \n"
    ;
  pbes p = txt2pbes(text);
  BOOST_CHECK(is_bes(p));
}
