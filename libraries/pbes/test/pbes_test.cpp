// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <utility>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "atermpp/make_list.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/pbes/pbes_translate.h"
#include "mcrl2/pbes/detail/tools.h"
#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/pbes/detail/quantifier_rename_builder.h"
#include "mcrl2/pbes/rename.h"

using namespace std;
using namespace atermpp;
using namespace lps;
using namespace lps::detail;

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
"init Controller(Normal,true,false,false,stopped);                                     \n";

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
  specification spec    = mcrl22lps(SPECIFICATION);
  state_formula formula = mcf2statefrm(FORMULA2, spec);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  pbes_expression e = p.equations().front().formula();

  BOOST_CHECK(!p.equations().is_bes());
  BOOST_CHECK(!e.is_bes());

  data_expression d  = pbes2data(e, spec);
  // pbes_expression e1 = data2pbes(d);
  // BOOST_CHECK(e == e1);

  try
  {
    p.load("non-existing file");
    BOOST_CHECK(false); // loading is expected to fail
  }
  catch (std::runtime_error e)
  {
  }

  try
  {
    aterm t = make_term("f(x)");
    std::string filename = "write_to_named_text_file.pbes";
    atermpp::write_to_named_text_file(t, filename);
    p.load(filename);
    BOOST_CHECK(false); // loading is expected to fail
  }
  catch (std::runtime_error e)
  {
  }
  p.save("pbes_test_file.pbes");
  p.load("pbes_test_file.pbes");
}

void test_normalize()
{
  using namespace state_frm;

  state_formula x = var(identifier_string("X"), data_expression_list());
  state_formula y = var(identifier_string("Y"), data_expression_list());
  state_formula f = imp(not_(x), y);
  state_formula f1 = normalize(f);
  state_formula f2 = or_(x, y);
  std::cout << "f  = " << pp(f) << std::endl;
  std::cout << "f1 = " << pp(f1) << std::endl;
  BOOST_CHECK(f1 == f2);


  specification mpsu_spec = mcrl22lps(MPSU_SPECIFICATION);
  state_formula mpsu_formula = mcf2statefrm(MPSU_FORMULA, mpsu_spec);
  bool timed = false;
  pbes p = lps2pbes(mpsu_spec, mpsu_formula, timed);
}

void test_xyz_generator()
{
  XYZ_identifier_generator generator(propositional_variable("X1(d:D)"));
  identifier_string x;
  x = generator(); BOOST_CHECK(std::string(x) == "X");
  x = generator(); BOOST_CHECK(std::string(x) == "Y");
  x = generator(); BOOST_CHECK(std::string(x) == "Z");
  x = generator(); BOOST_CHECK(std::string(x) == "X0");
  x = generator(); BOOST_CHECK(std::string(x) == "Y0");
  x = generator(); BOOST_CHECK(std::string(x) == "Z0");
  x = generator(); BOOST_CHECK(std::string(x) == "Y1"); // X1 should be skipped
}

void test_state_formula()
{
  specification spec    = mcrl22lps(SPECIFICATION);
  state_formula formula = mcf2statefrm("mu X. mu X. X", spec);
  std::map<identifier_string, identifier_string> replacements;
  set_identifier_generator generator(make_list(formula, spec));
  formula = remove_name_clashes_impl(formula, generator, replacements);
  std::cout << "formula: " << pp(formula) << std::endl;
  BOOST_CHECK(pp(formula) == "mu X. mu X00. X00");
}

void test_free_variables()
{
  pbes p;
  try {
    p.load("abp_fv.pbes");
    atermpp::set< data_variable > freevars = p.equations().free_variables();
    cout << freevars.size() << endl;
    BOOST_CHECK(freevars.size() == 20);
    for (atermpp::set< data_variable >::iterator i = freevars.begin(); i != freevars.end(); ++i)
    {
      cout << "<var>" << pp(*i) << endl;
    }
    freevars = p.equations().free_variables();
    BOOST_CHECK(freevars.size() == 15);
  }
  catch (std::runtime_error e)
  {
    cout << e.what() << endl;
    BOOST_CHECK(false); // loading is expected to succeed
  }
}

void test_pbes_expression_builder()
{
  specification mpsu_spec = mcrl22lps(MPSU_SPECIFICATION);
  state_formula mpsu_formula = mcf2statefrm(MPSU_FORMULA, mpsu_spec);
  bool timed = false;
  pbes p = lps2pbes(mpsu_spec, mpsu_formula, timed);

  for (equation_system::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
  {
    const pbes_expression& q = i->formula();
    pbes_expression_builder builder;
    pbes_expression q1 = builder.visit(q);
    BOOST_CHECK(q == q1);
  }
}

void test_quantifier_rename_builder()
{
  using namespace pbes_expr;
  namespace d = lps::data_expr; 

  data_variable mN("m:N");
  data_variable nN("n:N");

  pbes_expression f = d::equal_to(mN, nN);
  pbes_expression g = d::not_equal_to(mN, nN);

  multiset_identifier_generator generator(make_list(identifier_string("n00"), identifier_string("n01")));

  pbes_expression p1 = 
  and_(
    forall(make_list(nN), exists(make_list(nN), f)),
    forall(make_list(mN), exists(make_list(mN, nN), g))
  );
  pbes_expression q1 = make_quantifier_rename_builder(generator).visit(p1); 
  std::cout << "p1 = " << pp(p1) << std::endl;
  std::cout << "q1 = " << pp(q1) << std::endl;

  pbes_expression p2 = 
  and_(
    forall(make_list(nN), exists(make_list(nN), p1)),
    forall(make_list(mN), exists(make_list(mN, nN), q1))
  );
  // pbes_expression q2 = make_quantifier_rename_builder(generator).visit(p2); 
  pbes_expression q2 = rename_quantifier_variables(p2, make_list(data_variable("n00:N"), data_variable("n01:N")));
  std::cout << "p2 = " << pp(p2) << std::endl;
  std::cout << "q2 = " << pp(q2) << std::endl;

  // BOOST_CHECK(false);
}

int test_main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  test_pbes();
  test_normalize();
  test_state_formula();
  test_xyz_generator();
  // test_free_variables();
  test_pbes_expression_builder();
  test_quantifier_rename_builder();

  return 0;
}
