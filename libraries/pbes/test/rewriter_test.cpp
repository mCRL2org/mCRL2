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
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/pbes/pbes_translate.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/pbes/detail/quantifier_rename_builder.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rename.h"
#include "mcrl2/pbes/complement.h"
#include "mcrl2/pbes/normalize.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;
using namespace mcrl2::modal;
using namespace mcrl2::modal::detail;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;

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

const std::string ABP_SPECIFICATION =
"% This file contains the alternating bit protocol, as described in W.J.    \n"
"% Fokkink, J.F. Groote and M.A. Reniers, Modelling Reactive Systems.       \n"
"%                                                                          \n"
"% The only exception is that the domain D consists of two data elements to \n"
"% facilitate simulation.                                                   \n"
"                                                                           \n"
"sort                                                                       \n"
"  D     = struct d1 | d2;                                                  \n"
"  Error = struct e;                                                        \n"
"                                                                           \n"
"act                                                                        \n"
"  r1,s4: D;                                                                \n"
"  s2,r2,c2: D # Bool;                                                      \n"
"  s3,r3,c3: D # Bool;                                                      \n"
"  s3,r3,c3: Error;                                                         \n"
"  s5,r5,c5: Bool;                                                          \n"
"  s6,r6,c6: Bool;                                                          \n"
"  s6,r6,c6: Error;                                                         \n"
"  i;                                                                       \n"
"                                                                           \n"
"proc                                                                       \n"
"  S(b:Bool)     = sum d:D. r1(d).T(d,b);                                   \n"
"  T(d:D,b:Bool) = s2(d,b).(r6(b).S(!b)+(r6(!b)+r6(e)).T(d,b));             \n"
"                                                                           \n"
"  R(b:Bool)     = sum d:D. r3(d,b).s4(d).s5(b).R(!b)+                      \n"
"                  (sum d:D.r3(d,!b)+r3(e)).s5(!b).R(b);                    \n"
"                                                                           \n"
"  K             = sum d:D,b:Bool. r2(d,b).(i.s3(d,b)+i.s3(e)).K;           \n"
"                                                                           \n"
"  L             = sum b:Bool. r5(b).(i.s6(b)+i.s6(e)).L;                   \n"
"                                                                           \n"
"init                                                                       \n"
"  allow({r1,s4,c2,c3,c5,c6,i},                                             \n"
"    comm({r2|s2->c2, r3|s3->c3, r5|s5->c5, r6|s6->c6},                     \n"
"        S(true) || K || L || R(true)                                       \n"
"    )                                                                      \n"
"  );                                                                       \n"
;

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

const std::string REQ1_1 =
"nu X . [true] X &&                                                                     \n" 
"  forall nana:Node .                                                                   \n"
"    forall trans:Transaction .                                                         \n"
"      [cUQ(nana,trans)](nu Y . [true]Y && [exists nana2:Node . cUQ(nana2,trans)]false) \n"
; 

const std::string REQ1_2 =
"nu X . [true]X && forall n:Node . forall t:Transaction . [cUQ(n,t)](mu Y \n" 
". [!exists r:Phase . sUR(n,t,r)]Y && <true>true)                         \n"
;

void test_rewriter()
{
  specification spec    = mcrl22lps(SPECIFICATION);
  state_formula formula = mcf2statefrm(FORMULA2, spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  data::rewriter datar(spec.data());
  pbes_system::rewriter<data::rewriter> pbesr(datar, spec.data());
  for (atermpp::vector<pbes_equation>::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
  {
    pbes_expression e = i->formula();
    std::cout << pp(e) << " -> " << pp(pbesr(e)) << std::endl;
  }
}

/*
void test_normalize()
{
  using namespace pbes_expr;

  pbes_expression x = propositional_variable_instantiation("x:X");
  pbes_expression y = propositional_variable_instantiation("y:Y");
  pbes_expression z = propositional_variable_instantiation("z:Z");
  pbes_expression f; 
  pbes_expression f1;
  pbes_expression f2;

  f = not_(not_(x));
  f1 = normalize(f);
  f2 = x;
  std::cout << "f  = " << f  << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f = imp(not_(x), y);
  f1 = normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << f  << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = not_(and_(not_(x), not_(y)));
  f1 = normalize(f);
  f2 = or_(x, y);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = imp(and_(not_(x), not_(y)), z);
  f1 = normalize(f);
  f2 = or_(or_(x, y), z);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  x = data_variable("x:X");
  y = data_variable("y:Y");
  z = data_variable("z:Z");

  f  = not_(x);
  f1 = normalize(f);
  f2 = data_expr::not_(x);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  f  = imp(and_(x, y), z);
  f1 = normalize(f);
  f2 = or_(or_(data_expr::not_(x), data_expr::not_(y)), z);
  std::cout << "f  = " << f << std::endl;
  std::cout << "f1 = " << f1 << std::endl;
  std::cout << "f2 = " << f2 << std::endl;
  BOOST_CHECK(f1 == f2);

  pbes_expression T = true_();
  pbes_expression F = false_();
  x = pbes_expression(gsMakePBESImp(T, F));
  y = normalize(x);
  std::cout << "x = " << x << std::endl;
  std::cout << "y = " << y << std::endl;

  data_variable_list ab;
  ab = push_front(ab, data_variable("s:S"));
  x = propositional_variable_instantiation("x:X");
  y = and_(x, imp(pbes_expression(gsMakePBESAnd(false_(), false_())), false_()));
  z = normalize(y);
  std::cout << "y = " << y << std::endl;
  std::cout << "z = " << z << std::endl;
}

// void test_xyz_generator()
// {
//   XYZ_identifier_generator generator(propositional_variable("X1(d:D)"));
//   identifier_string x;
//   x = generator(); BOOST_CHECK(std::string(x) == "X");
//   x = generator(); BOOST_CHECK(std::string(x) == "Y");
//   x = generator(); BOOST_CHECK(std::string(x) == "Z");
//   x = generator(); BOOST_CHECK(std::string(x) == "X0");
//   x = generator(); BOOST_CHECK(std::string(x) == "Y0");
//   x = generator(); BOOST_CHECK(std::string(x) == "Z0");
//   x = generator(); BOOST_CHECK(std::string(x) == "Y1"); // X1 should be skipped
// }

void test_free_variables()
{
  pbes<> p;
  try {
    p.load("abp_fv.pbes");
    atermpp::set< data_variable > freevars = p.free_variables();
    cout << freevars.size() << endl;
    BOOST_CHECK(freevars.size() == 20);
    for (atermpp::set< data_variable >::iterator i = freevars.begin(); i != freevars.end(); ++i)
    {
      cout << "<var>" << pp(*i) << endl;
    }
    freevars = p.free_variables();
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
  pbes<> p = lps2pbes(mpsu_spec, mpsu_formula, timed);

  for (atermpp::vector<pbes_equation>::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
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
  namespace d = data_expr; 

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
  pbes_expression q2 = rename_quantifier_variables(p2, make_list(data_variable("n00:N"), data_variable("n01:N")));
  std::cout << "p2 = " << pp(p2) << std::endl;
  std::cout << "q2 = " << pp(q2) << std::endl;

  // BOOST_CHECK(false);
}

void test_complement_method_builder()
{
  using namespace pbes_expr;
  namespace d = data_expr;

  data_variable X("x:X");
  data_variable Y("y:Y");

  pbes_expression p = or_(and_(X,Y), and_(Y,X));
  pbes_expression q = and_(or_(d::not_(X), d::not_(Y)), or_(d::not_(Y),d::not_(X)));
  std::cout << "p             = " << pp(p) << std::endl;
  std::cout << "q             = " << pp(q) << std::endl;
  std::cout << "complement(p) = " << pp(complement(p)) << std::endl;
  BOOST_CHECK(complement(p) == q);
}

void test_pbes_expression()
{
  namespace p = pbes_expr;
  namespace d = data_expr;

  data_variable x1("x1:X");
  pbes_expression e = p::val(x1);
  data_expression x2 = p::val_arg(e);
  BOOST_CHECK(x1 == x2);
  
  pbes_expression v_expr = propositional_variable_instantiation("v:V");
  propositional_variable_instantiation v1 = v_expr;
  propositional_variable_instantiation v2(v_expr);
}

void test_trivial()
{
  specification spec    = mcrl22lps(ABP_SPECIFICATION);
  state_formula formula = mcf2statefrm(TRIVIAL_FORMULA, spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());
}

void test_lps2pbes()
{
  std::string SPECIFICATION;
  std::string FORMULA;
  specification spec;
  state_formula formula;
  pbes<> p;
  bool timed = false;

  SPECIFICATION =
  "act a;                                  \n"
  "proc X(n : Nat) = (n > 2) -> a. X(n+1); \n"
  "init X(3);                              \n"
  ; 
  FORMULA = "true => false";
  spec    = mcrl22lps(SPECIFICATION);
  formula = mcf2statefrm(FORMULA, spec);
  p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());

  SPECIFICATION =
  "act a : Nat;                           \n" 
  "proc X(n:Nat) = (n>2) -> a(n). X(n+1); \n"
  "init X(3);                             \n"
  ;
  FORMULA = "nu X. (X && forall m:Nat. [a(m)]false)";
  spec    = mcrl22lps(SPECIFICATION);
  formula = mcf2statefrm(FORMULA, spec);
  p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());

  SPECIFICATION =
  "act a;         \n"
  "proc X = a. X; \n"
  "init X;        \n"
  ; 
  FORMULA =
  "(                                 \n"
  "  ( mu A. [!a]A)                  \n"
  "||                                \n"
  "  ( mu B. exists t3:Pos . [!a]B ) \n"
  ")                                 \n"
  ;
  spec    = mcrl22lps(SPECIFICATION);
  formula = mcf2statefrm(FORMULA, spec);
  p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());

  specification model = mcrl22lps(MODEL);
  state_formula req1_1 = mcf2statefrm(REQ1_1, model);
  state_formula req1_2 = mcf2statefrm(REQ1_2, model);
  pbes<> p1 = lps2pbes(model, req1_1, timed);
  pbes<> p2 = lps2pbes(model, req1_2, timed);
}
*/

int test_main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  test_rewriter();

  return 0;
}
