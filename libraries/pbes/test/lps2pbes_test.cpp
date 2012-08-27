// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2pbes_test.cpp
/// \brief Add your file description here.

// Test program for timed lps2pbes.

//#define MCRL2_PBES_TRANSLATE_DEBUG
//#define MCRL2_STATE_FORMULA_BUILDER_DEBUG

#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/detail/test_utility.h"
#include "mcrl2/pbes/pbes_solver_test.h"
#include "test_specifications.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;
using namespace mcrl2::state_formulas;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;

const std::string TIMED_SPECIFICATION =
  "act a;\n"
  "proc P = a@1 . P;\n"
  "init P;\n"
;

const std::string TRIVIAL_FORMULA  = "[true*]<true*>true";

void test_trivial()
{
  specification spec    = linearise(lps::detail::ABP_SPECIFICATION());
  state_formula formula = state_formulas::parse_state_formula(TRIVIAL_FORMULA, spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());
  core::garbage_collect();
}

void test_timed()
{
  specification spec = linearise(TIMED_SPECIFICATION);
  state_formula formula = state_formulas::parse_state_formula(TRIVIAL_FORMULA, spec);
  bool timed = true;
  pbes<> p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());
  const atermpp::vector<sort_expression> user_def_sorts(p.data().user_defined_sorts());
  BOOST_CHECK(std::find(user_def_sorts.begin(), user_def_sorts.end(), sort_real::real_()) == user_def_sorts.end());

  const atermpp::vector<sort_expression> sorts(p.data().sorts());
  BOOST_CHECK(std::find(sorts.begin(), sorts.end(), sort_real::real_()) != sorts.end());
  core::garbage_collect();
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
  spec    = linearise(SPECIFICATION);
  formula = state_formulas::parse_state_formula(FORMULA, spec);
  p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());

  SPECIFICATION =
    "act a : Nat;                           \n"
    "proc X(n:Nat) = (n>2) -> a(n). X(n+1); \n"
    "init X(3);                             \n"
    ;
  FORMULA = "nu X. (X && forall m:Nat. [a(m)]false)";
  spec    = linearise(SPECIFICATION);
  formula = state_formulas::parse_state_formula(FORMULA, spec);
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
  spec    = linearise(SPECIFICATION);
  formula = state_formulas::parse_state_formula(FORMULA, spec);
  p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());

  SPECIFICATION =
    "sort Closure = List(Bool);                                                   \n"
    "sort State = struct state(closure: Closure, copy: Nat);                      \n"
    "                                                                             \n"
    "map initial: State -> Bool;                                                  \n"
    "var q: State;                                                                \n"
    "eqn initial(q) = closure(q).0 && (copy(q) == 0);                             \n"
    "                                                                             \n"
    "map accept: State -> Bool;                                                   \n"
    "var q: State;                                                                \n"
    "eqn accept(q) = ((copy(q) == 0) && (closure(q).0 => closure(q).2));          \n"
    "                                                                             \n"
    "map nextstate: State # State -> Bool;                                        \n"
    "var q, q': State;                                                            \n"
    "eqn nextstate(q, q') =                                                       \n"
    "      (#closure(q) == #closure(q')) &&                                       \n"
    "      (accept(q) => (copy(q') == (copy(q) + 1) mod 1)) &&                    \n"
    "      (!accept(q) => (copy(q') == copy(q))) &&                               \n"
    "      (closure(q).0 == (closure(q).2 || (closure(q).1 && closure(q').0))) && \n"
    "      (closure(q').0 => closure(q').1 || closure(q').2) &&                   \n"
    "      (closure(q').2 => closure(q').0);                                      \n"
    "                                                                             \n"
    "act a, b;                                                                    \n"
    "proc P(s: Bool) =  s -> (a . P(false)) <> delta +                            \n"
    "                  !s -> (b . P(s)) <> delta;                                 \n"
    "init P(true);                                                                \n"
    ;

  FORMULA =
    "forall c1: State .                                                                                                                                            \n"
    " (exists c0: State .                                                                                                                                          \n"
    "   (val(initial(c0) && nextstate(c0, c1)) &&                                                                                                                  \n"
    "    (((<a>true) => val(closure(c1).1)) && (val(closure(c1).1) => (<a>true)) && ((<b>true) => val(closure(c1).2)) && (val(closure(c1).2) => (<b>true)))        \n"
    "   )                                                                                                                                                          \n"
    " ) => (                                                                                                                                                       \n"
    "   mu X(c'': State = c1) . (                                                                                                                                  \n"
    "     nu Y(c: State = c'') . (                                                                                                                                 \n"
    "       forall c': State . (                                                                                                                                   \n"
    "         val(nextstate(c, c')) =>                                                                                                                             \n"
    "         [true](                                                                                                                                              \n"
    "           (((<a>true) => val(closure(c).1)) && (val(closure(c).1) => (<a>true)) && ((<b>true) => val(closure(c).2)) && (val(closure(c).2) => (<b>true))) =>  \n"
    "           ((val(accept(c)) && X(c')) || (val(!accept(c)) && Y(c')))                                                                                          \n"
    "         )                                                                                                                                                    \n"
    "       )                                                                                                                                                      \n"
    "     )                                                                                                                                                        \n"
    "   )                                                                                                                                                          \n"
    " )                                                                                                                                                            \n"
    ;

  spec    = linearise(SPECIFICATION);
  formula = state_formulas::parse_state_formula(FORMULA, spec);
  p = lps2pbes(spec, formula, timed);
  BOOST_CHECK(p.is_well_typed());

  core::garbage_collect();
}

void test_lps2pbes2()
{
  std::string FORMULA;
  pbes<> p;
  bool timed = false;

  FORMULA = "mu X. !!X";
  p = lps2pbes(lps::detail::ABP_SPECIFICATION(), FORMULA, timed);
  BOOST_CHECK(p.is_well_typed());

  FORMULA = "nu X. ([true]X && <true>true)";
  p = lps2pbes(lps::detail::ABP_SPECIFICATION(), FORMULA, timed);
  BOOST_CHECK(p.is_well_typed());

  FORMULA = "nu X. ([true]X && forall d:D. [r1(d)] mu Y. (<true>Y || <s4(d)>true))";
  p = lps2pbes(lps::detail::ABP_SPECIFICATION(), FORMULA, timed);
  BOOST_CHECK(p.is_well_typed());

  FORMULA = "forall d:D. nu X. (([!r1(d)]X && [s4(d)]false))";
  p = lps2pbes(lps::detail::ABP_SPECIFICATION(), FORMULA, timed);
  BOOST_CHECK(p.is_well_typed());

  FORMULA = "nu X. ([true]X && forall d:D. [r1(d)]nu Y. ([!r1(d) && !s4(d)]Y && [r1(d)]false))";
  p = lps2pbes(lps::detail::ABP_SPECIFICATION(), FORMULA, timed);
  BOOST_CHECK(p.is_well_typed());
  core::garbage_collect();
}

void test_lps2pbes3()
{
  std::string SPEC = "init delta;";

  std::string FORMULA =
    "(mu X(n:Nat = 0) . true) \n"
    "&&                       \n"
    "(mu X(n:Nat = 0) . true) \n"
    ;

  // Expected result:
  //
  // pbes nu X1 =
  //        Y(0) && X(0);
  //      mu Y(n: Nat) =
  //        true;
  //      mu X(n: Nat) =
  //        true;
  //
  // init X1;

  pbes<> p;
  bool timed = false;
  p = lps2pbes(SPEC, FORMULA, timed);
  BOOST_CHECK(p.is_well_typed());
  std::cerr << "p = " << pbes_system::pp(p) << std::endl;
  core::garbage_collect();
}

// Trac ticket #841, example supplied by Tim Willemse.
void test_lps2pbes4()
{
  std::string SPEC =
    "act  a: Nat;                    \n"
    "                                \n"
    "proc P(s3: Pos, n: Nat) =       \n"
    "       sum m: Nat.              \n"
    "         (s3 == 1) ->           \n"
    "         a(m) .                 \n"
    "         P(s3 = 2, n = m)       \n"
    "     + sum m0: Nat.             \n"
    "         (s3 == 2 && n < m0) -> \n"
    "         a(m0) .                \n"
    "         P(s3 = 2, n = m0)      \n"
    "     + delta;                   \n"
    "                                \n"
    "init P(1, 0);                   \n"
    ;

  std::string FORMULA = "nu X(n :Nat=0). X(n+1)";

  lps::specification spec = lps::parse_linear_process_specification(SPEC);
  state_formulas::state_formula formula = state_formulas::parse_state_formula(FORMULA, spec);
  std::cout << "formula = " << state_formulas::pp(formula) << std::endl;
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  std::cerr << "p = " << pbes_system::pp(p) << std::endl;
  BOOST_CHECK(p.is_well_typed());
  core::garbage_collect();
}

#ifdef MCRL2_USE_BOOST_FILESYSTEM
void test_directory(int argc, char** argv)
{
  BOOST_CHECK(argc > 1);

  // The dummy file test.test is used to extract the full path of the test directory.
  fs::path dummy_path = fs::system_complete(fs::path(argv[1], fs::native));
  fs::path dir = dummy_path.branch_path();
  BOOST_CHECK(fs::is_directory(dir));

  fs::directory_iterator end_iter;
  for (fs::directory_iterator dir_itr(dir); dir_itr != end_iter; ++dir_itr)
  {
    if (fs::is_regular(dir_itr->status()))
    {
      std::string filename = dir_itr->path().file_string();
      if (boost::ends_with(filename, std::string(".form")))
      {
        std::string timed_result_file   = filename.substr(0, filename.find_last_of('.') + 1) + "expected_timed_result";
        std::string untimed_result_file = filename.substr(0, filename.find_last_of('.') + 1) + "expected_untimed_result";
        std::string formula = mcrl2::utilities::read_text(filename);
        if (fs::exists(timed_result_file))
        {
          try
          {
            pbes<> result = lps2pbes(SPEC1, formula, true);
            pbes<> expected_result;
            expected_result.load(timed_result_file);
            bool cmp = (result == expected_result);
            if (!cmp)
            {
              cerr << "ERROR: test " << timed_result_file << " failed!" << endl;
            }
            BOOST_CHECK(cmp);
          }
          catch (mcrl2::runtime_error e)
          {
            cerr << e.what() << endl;
          }
        }
        if (fs::exists(untimed_result_file))
        {
          try
          {
            pbes<> result = lps2pbes(SPEC1, formula, false);
            BOOST_CHECK(result.is_well_typed());
            pbes<> expected_result;
            expected_result.load(untimed_result_file);
            bool cmp = (result == expected_result);
            if (!cmp)
            {
              cerr << "ERROR: test " << untimed_result_file << " failed!" << endl;
            }
            BOOST_CHECK(cmp);
          }
          catch (mcrl2::runtime_error e)
          {
            cerr << e.what() << endl;
          }
        }
      }
    }
  }
  core::garbage_collect();
}
#endif

void test_formulas()
{
  std::string SPEC =
    "act a:Nat;                             \n"
    "map smaller: Nat#Nat -> Bool;          \n"
    "var x,y : Nat;                         \n"
    "eqn smaller(x,y) = x < y;              \n"
    "proc P(n:Nat) = sum m: Nat. a(m). P(m);\n"
    "init P(0);                             \n"
    ;

  std::vector<string> formulas;
  formulas.push_back("delay@11");
  formulas.push_back("exists m:Nat. <a(m)>true");
  formulas.push_back("exists p:Nat. <a(p)>true");
  formulas.push_back("forall m:Nat. [a(m)]false");
  formulas.push_back("nu X(n:Nat = 1). [forall m:Nat. a(m)](val(n < 10)  && X(n+2))");
  formulas.push_back("mu X(n:Nat = 1). [forall m:Nat. a(m)](val(smaller(n,10) ) && X(n+2))");
  formulas.push_back("<exists m:Nat. a(m)>true");
  formulas.push_back("<a(2)>[a(0)]false");
  formulas.push_back("<a(2)>true");
  formulas.push_back("[forall m:Nat. a(m)]false");
  formulas.push_back("[a(0)]<a(1)>true");
  formulas.push_back("[a(1)]false");
  formulas.push_back("!true");
  formulas.push_back("yaled@10");

  for (std::vector<string>::iterator i = formulas.begin(); i != formulas.end(); ++i)
  {
    std::cout << "<formula>" << *i << std::flush;
    pbes<> result1 = lps2pbes(SPEC, *i, false);
    std::cout << " <timed>" << std::flush;
    pbes<> result2 = lps2pbes(SPEC, *i, true);
    std::cout << " <untimed>" << std::endl;
  }
  core::garbage_collect();
}

const std::string MACHINE_SPECIFICATION =
  "%% file machine.mcrl2                                             \n"
  "                                                                  \n"
  "act                                                               \n"
  "                                                                  \n"
  "  ch_tea, ch_cof, insq, insd, take_tea, take_cof, want_change,    \n"
  "  sel_tea, sel_cof, accq, accd, put_tea, put_cof, put_change,     \n"
  "  ok_tea, ok_coffee, quarter, dollar, tea, coffee, change ;       \n"
  "                                                                  \n"
  "proc                                                              \n"
  "                                                                  \n"
  "  User = ch_tea.UserTea + ch_cof.UserCof ;                        \n"
  "  UserTea =                                                       \n"
  "    insq.insq.take_tea.User  +                                    \n"
  "    insd.take_tea.( take_tea + want_change ).User ;               \n"
  "  UserCof = ( insq.insq.insq.insq + insd ).take_cof.User ;        \n"
  "                                                                  \n"
  "  Mach = sel_tea.MachTea + sel_cof.MachCof ;                      \n"
  "  MachTea =                                                       \n"
  "    accq.accq.put_tea.Mach +                                      \n"
  "    accd.put_tea.( put_tea + put_change ).Mach ;                  \n"
  "  MachCof =                                                       \n"
  "    ( accq.accq.accq.accq + accd ).put_cof.Mach ;                 \n"
  "                                                                  \n"
  "init                                                              \n"
  "                                                                  \n"
  "  allow(                                                          \n"
  "    { ok_tea, ok_coffee, quarter, dollar, tea, coffee, change } , \n"
  "    comm(                                                         \n"
  "       { ch_tea|sel_tea -> ok_tea, ch_cof|sel_cof -> ok_coffee,   \n"
  "         insq|accq -> quarter, insd|accd -> dollar,               \n"
  "         take_tea|put_tea -> tea, take_cof|put_cof -> coffee,     \n"
  "         want_change|put_change -> change } ,                     \n"
  "       User || Mach                                               \n"
  "    ) ) ;                                                         \n"
  ;

const std::string MACHINE_FORMULA1 =
  "%% after choice for tea and two quarter always tea (true) \n"
  " [ true* . ok_tea . quarter . quarter . !tea ] false      \n"
  ;

const std::string MACHINE_FORMULA2 =
  "%% always eventually action ready (true)     \n"
  " [ true* . dollar . !(tea||coffee) ] false   \n"
  ;

const std::string MACHINE_FORMULA3 =
  "%% after a quarter no change directly (true) \n"
  " [ true* . quarter . change ] false          \n"
  ;

void test_lps2pbes(std::string lps_spec, std::string mcf_formula)
{
  using namespace pbes_system;

  lps::specification spec = lps::linearise(lps_spec);
  state_formulas::state_formula formula = state_formulas::parse_state_formula(mcf_formula, spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  core::garbage_collect();
}

// Submitted by Tim, 2-9-2010
void test_example()
{
  std::string SPEC =
    "act a,b;                                         \n"
    "                                                 \n"
    "proc S = sum n:Nat. (n < 3) -> a.X(n);           \n"
    "                                                 \n"
    "proc X(n:Nat) = (n == 0) -> ( (a+b).X(n))        \n"
    "               +(n > 0 ) -> b.a.X(Int2Nat(n-1)); \n"
    "                                                 \n"
    "init S;                                          \n"
    ;

  std::string FORMULA = "<a>([a]false)";

  pbes<> p;
  bool timed = false;
  p = lps2pbes(SPEC, FORMULA, timed);
  BOOST_CHECK(p.is_well_typed());
  std::cerr << "p = " << pbes_system::pp(p) << std::endl;

  bool result = pbes2_bool_test(p);
  BOOST_CHECK(result == true);

  core::garbage_collect();
}

// Submitted by Jeroen Keiren, 10-09-2010
// Formula 2 and 3 give normalization errors.
void test_elevator()
{
  std::string SPEC =
    "% Model of an elevator for n floors.                                                                                           \n"
    "% Originally described in 'Solving Parity Games in Practice' by Oliver                                                         \n"
    "% Friedmann and Martin Lange.                                                                                                  \n"
    "%                                                                                                                              \n"
    "% This is the version with a first in first out policy                                                                         \n"
    "                                                                                                                               \n"
    "sort Floor = Pos;                                                                                                              \n"
    "     DoorStatus = struct open | closed;                                                                                        \n"
    "     Requests = List(Floor);                                                                                                   \n"
    "                                                                                                                               \n"
    "map maxFloor: Floor;                                                                                                           \n"
    "eqn maxFloor = 3;                                                                                                              \n"
    "                                                                                                                               \n"
    "map addRequest : Requests # Floor -> Requests;                                                                                 \n"
    "                                                                                                                               \n"
    "var r: Requests;                                                                                                               \n"
    "    f,g: Floor;                                                                                                                \n"
    "    % FIFO behaviour!                                                                                                          \n"
    "eqn addRequest([], f) = [f];                                                                                                   \n"
    "    (f == g) -> addRequest(g |> r, f) = g |> r;                                                                                \n"
    "    (f != g) -> addRequest(g |> r, f) = g |> addRequest(r, f);                                                                 \n"
    "                                                                                                                               \n"
    "map removeRequest : Requests -> Requests;                                                                                      \n"
    "var r: Requests;                                                                                                               \n"
    "    f: Floor;                                                                                                                  \n"
    "eqn removeRequest(f |> r) = r;                                                                                                 \n"
    "                                                                                                                               \n"
    "map getNext : Requests -> Floor;                                                                                               \n"
    "var r: Requests;                                                                                                               \n"
    "    f: Floor;                                                                                                                  \n"
    "eqn getNext(f |> r) = f;                                                                                                       \n"
    "                                                                                                                               \n"
    "act isAt: Floor;                                                                                                               \n"
    "    request: Floor;                                                                                                            \n"
    "    close, open, up, down;                                                                                                     \n"
    "                                                                                                                               \n"
    "proc Elevator(at: Floor, status: DoorStatus, reqs: Requests, moving: Bool) =                                                   \n"
    "       isAt(at) . Elevator()                                                                                                   \n"
    "     + sum f: Floor. (f <= maxFloor) -> request(f) . Elevator(reqs = addRequest(reqs, f))                                      \n"
    "     + (status == open) -> close . Elevator(status = closed)                                                                   \n"
    "     + (status == closed && reqs != [] && getNext(reqs) > at) -> up . Elevator(at = at + 1, moving = true)                     \n"
    "     + (status == closed && reqs != [] && getNext(reqs) < at) -> down . Elevator(at = Int2Pos(at - 1), moving = true)          \n"
    "     + (status == closed && getNext(reqs) == at) -> open. Elevator(status = open, reqs = removeRequest(reqs), moving = false); \n"
    "                                                                                                                               \n"
    "init Elevator(1, open, [], false);                                                                                             \n"
    ;

  std::string formula1 = "nu U. [true] U && ((mu V . nu W. !([!request(maxFloor)]!W && [request(maxFloor)]!V)) || (nu X . mu Y. [!isAt(maxFloor)] Y &&  [isAt(maxFloor)]X))";
  std::string formula2 = "nu U. [true] U && ((nu V . mu W. ([!request(maxFloor)]W && [request(maxFloor)]V)) => (nu X . mu Y. [!isAt(maxFloor)] Y &&  [isAt(maxFloor)]X))";
  std::string formula3 = "nu U. [true] U && (!(nu V . mu W. ([!request(maxFloor)]W && [request(maxFloor)]V)) || (nu X . mu Y. [!isAt(maxFloor)] Y &&  [isAt(maxFloor)]X))";
  std::string formula4 = "(nu V . mu W. V) => true";
  std::string formula5 = "!(nu V . mu W. V)";

  pbes<> p;
  bool timed = false;
  p = lps2pbes(SPEC, formula1, timed);
  p = lps2pbes(SPEC, formula2, timed);
  p = lps2pbes(SPEC, formula3, timed);
  p = lps2pbes(SPEC, formula4, timed);

  core::garbage_collect();
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_elevator();
  test_example();
  test_lps2pbes();
  test_lps2pbes2();
  test_lps2pbes3();
  test_lps2pbes4();
  test_trivial();
  test_formulas();
  test_timed();

#ifdef MCRL2_EXTENDED_TESTS
  test_lps2pbes(MACHINE_SPECIFICATION, MACHINE_FORMULA1);
  test_lps2pbes(MACHINE_SPECIFICATION, MACHINE_FORMULA2);
  test_lps2pbes(MACHINE_SPECIFICATION, MACHINE_FORMULA3);
#endif

  return 0;
}
