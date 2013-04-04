// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parity_game_test.cpp
/// \brief Test for parity game solver. N.B. Currently no real
/// testing is done. Instead graph representations are produced
/// in a format that can be read by a python script.

//#define MCRL2_PARITY_GAME_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
//#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <fstream>
#include <string>
#include <iomanip>
#include <boost/test/minimal.hpp>
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/detail/test_input.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/pbes_solver_test.h"
#include "mcrl2/pbes/print.h"
#include "mcrl2/pbes/detail/parity_game_output.h"

#ifdef MCRL2_PGSOLVER_ENABLED
#include <cstdlib>
#endif

using namespace mcrl2;

std::string BES1 =
  "pbes mu X = X;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES2 =
  "pbes nu X = X;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES3 =
  "pbes mu X = Y;                                           \n"
  "     nu Y = X;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES4 =
  "pbes nu Y = X;                                           \n"
  "     mu X = Y;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES5 =
  "pbes mu X1 = X2;                                         \n"
  "     nu X2 = X1 || X3;                                   \n"
  "     mu X3 = X4 && X5;                                   \n"
  "     nu X4 = X1;                                         \n"
  "     nu X5 = X1 || X3;                                   \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string BES6 =
  "pbes nu X1 = X2 && X1;                                   \n"
  "     mu X2 = X1 || X3;                                   \n"
  "     nu X3 = X3;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string BES7 =
  "pbes nu X1 = X2 && X3;                                   \n"
  "     nu X2 = X4 && X5;                                   \n"
  "     nu X3 = true;                                       \n"
  "     nu X4 = false;                                      \n"
  "     nu X5 = X6;                                         \n"
  "     nu X6 = X5;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string BES8 =
  "pbes nu X1 = X2 && X1;                                   \n"
  "     mu X2 = X1;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string PBES1 =
  "pbes mu X(m: Nat) =                          \n"
  "       forall n: Nat. val(!(n < 3)) && X(n); \n"
  "                                             \n"
  "init X(0);                                   \n"
  ;

std::string PBES2 =
  "pbes                                                 \n"
  "mu X(m:Nat) = !(exists n:Nat.(val(n < 3) || !X(n))); \n"
  "                                                     \n"
  "init X(0);                                           \n"
  ;

std::string PBES3 =
  "pbes                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \n"
  "nu X0 = ((!(forall m:Nat.((val(m < 3)) && (!X2(m + 1, 0))))) || (((forall k:Nat.((val(k < 3)) && (val(k < 2)))) || (exists n:Nat.((val(n < 3)) || (val(n < 2))))) || ((val(true)) => (exists k:Nat.((val(k < 3)) || (exists m:Nat.((val(m < 3)) || (forall k:Nat.((val(k < 3)) && (X4(1, m + 1))))))))))) && ((!(exists n:Nat.((val(n < 3)) || (exists k:Nat.((val(k < 3)) || (val(false))))))) || (forall n:Nat.((val(n < 3)) && (exists n:Nat.((val(n < 3)) || (forall m:Nat.((val(m < 3)) && (exists k:Nat.((val(k < 3)) || (X3)))))))))); \n"
  "mu X1(b:Bool) = ((exists m:Nat.((val(m < 3)) || (val(b)))) => ((val(false)) || (X3))) && (forall k:Nat.((val(k < 3)) && ((((val(k > 1)) => (X1(k > 1))) && (val(false))) && (exists k:Nat.((val(k < 3)) || (X4(k + 1, k + 1)))))));                                                                                                                                                                                                                                                                                                           \n"
  "mu X2(n:Nat, m:Nat) = ((exists k:Nat.((val(k < 3)) || (!(val(n < 2))))) && ((X3) || ((forall k:Nat.((val(k < 3)) && (!((val(m < 3)) || (val(m == k)))))) || (!((X0) => (!X4(n + 1, 1))))))) && (exists n:Nat.((val(n < 3)) || (forall m:Nat.((val(m < 3)) && (exists k:Nat.((val(k < 3)) || (val(k < 3))))))));                                                                                                                                                                                                                               \n"
  "mu X3 = ((val(true)) || (X3)) || (!((forall n:Nat.((val(n < 3)) && (!((val(true)) || ((!(val(n > 1))) && ((X1(n < 3)) || (val(n > 1)))))))) && (forall n:Nat.((val(n < 3)) && (exists m:Nat.((val(m < 3)) || (!X0)))))));                                                                                                                                                                                                                                                                                                                     \n"
  "mu X4(n:Nat, m:Nat) = ((((forall m:Nat.((val(m < 3)) && ((val(m < 2)) || (val(n < 2))))) => (X1(true))) || (forall m:Nat.((val(m < 3)) && (exists m:Nat.((val(m < 3)) || (val(n > 0))))))) || (forall n:Nat.((val(n < 3)) && ((!(forall n:Nat.((val(n < 3)) && (!X4(0, 1))))) || (forall k:Nat.((val(k < 3)) && (val(true)))))))) || (exists m:Nat.((val(m < 3)) || (forall n:Nat.((val(n < 3)) && (!(!X0))))));                                                                                                                              \n"
  "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \n"
  "init X0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \n"
  ;

void test_pbes(std::string pbes_spec)
{
  pbes_system::pbes p = pbes_system::txt2pbes(pbes_spec);
  pbes_system::detail::parity_game_output pgg(p);
  pgg.run();

// check the result
#ifdef MCRL2_PGSOLVER_ENABLED
  static int index;
  bool expected_result = pbes_system::pbes2_bool_test(p);
  std::string name = "parity_game_test" + boost::lexical_cast<std::string>(++index);
  std::string output_file   = name + ".pg";
  std::string solution_file = name + ".solution";
  std::string text = pgg.pgsolver_graph();
  std::ofstream to(output_file.c_str());
  to << text << std::endl;
  std::string command = "pgsolver --recursive --solonly " + output_file + " > " + solution_file;
  std::cout << "executing: " << command << std::endl;
  int execution_result = std::system(command.c_str());
  BOOST_CHECK(execution_result == 0);
  std::string solution = utilities::read_text(solution_file);
  std::istringstream is(solution);

  // skip the first line
  std::string dummy;
  std::getline(is, dummy);

  int player;
  int winner;
  is >> player >> winner;
  std::cout << "player = " << player << std::endl;
  std::cout << "winner = " << winner << std::endl;
  BOOST_CHECK(player == 0);
  BOOST_CHECK(expected_result = (winner == 0));
#endif
}

// mimick the way parity_game_generator is used in parity game solver from Twente
void test_pbespgsolve(std::string pbes_spec)
{
  pbes_system::pbes p = pbes_system::txt2pbes(pbes_spec);
  pbes_system::parity_game_generator pgg(p, true, true);
  size_t begin = 0;
  size_t end = 3;
  for (size_t v = begin; v < end; ++v)
  {
    std::set<size_t> deps = pgg.get_dependencies(v);
    for (std::set<size_t>::const_iterator it = deps.begin(); it != deps.end(); ++it)
    {
      size_t w = *it;
      assert(w >= begin);
      if (w >= end)
      {
        end = w + 1;
      }
      // edges.push_back(std::make_pair(v - begin, w - begin));
    }

    int max_prio = 0;
    for (size_t v = begin; v < end; ++v)
    {
      max_prio = (std::max)(max_prio, (int)pgg.get_priority(v));
    }

    for (size_t v = begin; v < end; ++v)
    {
      // Variable below is not used; So, I removed it to avoid warnings. JFG.
      // bool and_op = pgg.get_operation(v) == mcrl2::pbes_system::parity_game_generator::PGAME_AND;
      pgg.get_priority(v);
    }
  }
}

void test_lps(const std::string& lps_spec, const std::string& formula = lps::detail::NO_DEADLOCK())
{
  using namespace pbes_system;
  bool timed = false;
  pbes p = lps2pbes(lps_spec, formula, timed);
  std::string text = pbes_system::pp(p);
  test_pbes(text);
}

int test_main(int argc, char** argv)
{
  test_pbes(BES1);
  test_pbes(BES2);
  test_pbes(BES3);
  test_pbes(BES4);
  test_pbes(BES5);
  test_pbes(BES6);
  test_pbes(BES7);
  test_pbes(BES8);
  test_pbes(PBES1);
  test_pbes(PBES2);
  test_pbes(PBES3);

  test_pbespgsolve(PBES1);
  test_pbespgsolve(PBES2);
  test_pbespgsolve(PBES3);

  test_lps(lps::detail::ABP_SPECIFICATION());

#ifdef MCRL2_EXTENDED_TESTS
  test_lps(lps::detail::DINING3_SPECIFICATION());
  test_lps(lps::detail::ONE_BIT_SLIDING_WINDOW_SPECIFICATION(), "nu X. <true>true && [true]X");
#endif

  return 0;
}
