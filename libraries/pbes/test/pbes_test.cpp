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

#define MCRL2_DEBUG_ENUMERATE_QUANTIFIERS_BUILDER

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/complement.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/is_bes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/txt2pbes.h"
#include <boost/algorithm/string.hpp>
#include <boost/test/minimal.hpp>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <utility>

using namespace mcrl2;
using core::identifier_string;
using data::data_expression;
using data::variable;
using data::basic_sort;
using data::multiset_identifier_generator;
using state_formulas::state_formula;
using lps::linearise;
using lps::specification;
using pbes_system::pbes;
using pbes_system::pbes_expression;
using pbes_system::pbes_equation;
using pbes_system::lps2pbes;
using pbes_system::propositional_variable_instantiation;

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

const std::string TRIVIAL_FORMULA  = "[true*]<true*>true";

const std::string MPSU_SPECIFICATION =
  "% This file describes a controller for a simplified Movable Patient                   \n"
  "% Support Unit. It is described in Fokkink, Groote and Reniers,                       \n"
  "% Modelling reactive systems.                                                         \n"
  "%                                                                                     \n"
  "% Jan Friso Groote, September, 2006.                                                  \n"
  "                                                                                      \n"
  "sort Mode = struct Normal | Emergency ;                                               \n"
  "     MotorStatus = struct turnleft | turnright | stopped ;                            \n"
  "                                                                                      \n"
  "act pressStop, pressResume,                                                           \n"
  "    pressUndock, pressLeft,                                                           \n"
  "    pressRight, motorLeft,                                                            \n"
  "    motorRight, motorOff,                                                             \n"
  "    applyBrake, releaseBrake,                                                         \n"
  "    isDocked, unlockDock,                                                             \n"
  "    atInnermost, atOutermost;                                                         \n"
  "                                                                                      \n"
  "proc Controller(m:Mode,docked,rightmost,leftmost:Bool,ms:MotorStatus)=                \n"
  "       pressStop.Controller(Emergency,docked,rightmost,leftmost,ms)+                  \n"
  "       pressResume.Controller(Normal,docked,rightmost,leftmost,ms)+                   \n"
  "       pressUndock.                                                                   \n"
  "         (docked && rightmost)                                                        \n"
  "                -> applyBrake.unlockDock.Controller(m,false,rightmost,leftmost,ms)    \n"
  "                <> Controller(m,docked,rightmost,leftmost,ms)+                        \n"
  "       pressLeft.                                                                     \n"
  "          (docked && ms!=turnleft && !leftmost && m==Normal)                          \n"
  "                -> releaseBrake.motorLeft.                                            \n"
  "                     Controller(m,docked,false,leftmost,turnleft)                     \n"
  "                <> Controller(m,docked,rightmost,leftmost,ms)+                        \n"
  "       pressRight.                                                                    \n"
  "          (docked && ms!=turnright && !rightmost && m==Normal)                        \n"
  "                -> releaseBrake.motorRight.                                           \n"
  "                     Controller(m,docked,rightmost,false,turnright)                   \n"
  "                <> Controller(m,docked,rightmost,leftmost,ms)+                        \n"
  "       isDocked.Controller(m,true,rightmost,leftmost,ms)+                             \n"
  "       atInnermost.motorOff.applyBrake.Controller(m,docked,true,false,stopped)+       \n"
  "       atOutermost.motorOff.applyBrake.Controller(m,docked,false,true,stopped);       \n"
  "                                                                                      \n"
  "                                                                                      \n"
  "                                                                                      \n"
  "init Controller(Normal,true,false,false,stopped);                                     \n"
  ;

const std::string FORMULA  = "nu X(n:Nat = 1). [forall m:Nat. a(m)](val(n < 10)  && X(n+2))";
const std::string FORMULA2 = "forall m:Nat. [a(m)]false";

const std::string MPSU_FORMULA =
  "% This file describes the modal formulas for property 5 used in section \n"
  "% 5.3 of Designing and understanding the behaviour of systems           \n"
  "% by J.F. Groote and M.A. Reniers.                                      \n"
  "                                                                        \n"
  "nu X(b1:Bool=false, b2:Bool=true,b3:Bool=true,b4:Bool=true).            \n"
  "        val(b1 && b2 && b3 && b4) => ([pressLeft]                       \n"
  "              (mu Y.[!motorLeft &&                                      \n"
  "                    !unlockDock &&                                      \n"
  "                    !pressStop &&                                       \n"
  "                    !atInnermost]Y))                                    \n";

void test_pbes()
{
  specification spec=remove_stochastic_operators(linearise(SPECIFICATION));
  state_formula formula = state_formulas::parse_state_formula(FORMULA2, spec);
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

void test_global_variables()
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
  std::set<variable> freevars = p.global_variables();
  BOOST_CHECK(freevars.size() == 3);  // The global variable k does not occur in the specification,
  // but occurs in the global variables list.
}

void test_complement_method_builder()
{
  using namespace pbes_system;
  namespace d = data::sort_bool;

  variable X("x", data::sort_bool::bool_());
  variable Y("y", data::sort_bool::bool_());

  pbes_expression p = or_(and_(X,Y), and_(Y,X));
  pbes_expression q = and_(or_(d::not_(X), d::not_(Y)), or_(d::not_(Y),d::not_(X)));
  std::cout << "p             = " << mcrl2::pbes_system::pp(p) << std::endl;
  std::cout << "q             = " << mcrl2::pbes_system::pp(q) << std::endl;
  std::cout << "complement(p) = " << mcrl2::pbes_system::pp(complement(p)) << std::endl;
  BOOST_CHECK(complement(p) == q);
}

void test_pbes_expression()
{
  variable x1("x1", basic_sort("X"));
  pbes_expression e = x1;
}

void test_trivial()
{
  specification spec=remove_stochastic_operators(linearise(lps::detail::ABP_SPECIFICATION()));
  state_formula formula = state_formulas::parse_state_formula(TRIVIAL_FORMULA, spec);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());
}

void test_instantiate_global_variables()
{
  std::string spec_text =
    "act a,b:Nat;             \n"
    "    d;                   \n"
    "proc P(n:Nat)=a(n).delta;\n"
    "init d.P(1);             \n"
    ;
  std::string formula_text = "([true*.a(1)]  (mu X.([!a(1)]X && <true> true)))";
  specification spec=remove_stochastic_operators(linearise(spec_text));
  state_formula formula = state_formulas::parse_state_formula(formula_text, spec);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  std::cout << "<before>" << mcrl2::pbes_system::pp(p) << std::endl;
  std::cout << "<lps>" << lps::pp(spec) << std::endl;
  pbes_system::detail::instantiate_global_variables(p);
  std::cout << "<after>" << pbes_system::pp(p) << std::endl;
}

void test_find_sort_expressions()
{
  using data::sort_expression;

  specification spec=remove_stochastic_operators(linearise(lps::detail::ABP_SPECIFICATION()));
  state_formula formula = state_formulas::parse_state_formula(TRIVIAL_FORMULA, spec);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  std::set<sort_expression> s;
  pbes_system::find_sort_expressions(p, std::inserter(s, s.end()));
  std::cout << core::detail::print_set(s) << std::endl;
}

void test_io()
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

void test_is_bes()
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

int test_main(int argc, char** argv)
{
  test_trivial();
  test_pbes();
  test_global_variables();
  test_complement_method_builder();
  test_pbes_expression();
  test_instantiate_global_variables();
  test_find_sort_expressions();
  test_is_bes();
  test_io();

  return 0;
}
