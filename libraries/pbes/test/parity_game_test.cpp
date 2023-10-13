// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parity_game_test.cpp
/// \brief Test for parity game solver. N.B. Currently no real
/// testing is done. Instead graph representations are produced
/// in a format that can be read by a python script.

#define BOOST_TEST_MODULE parity_game_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/detail/test_input.h"
#include "mcrl2/pbes/detail/parity_game_output.h"
#include "mcrl2/pbes/detail/pbessolve.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/print.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;

void test_pbes(const std::string& pbes_spec, const bool expected_result)
{
  (void)expected_result;
  
  pbes_system::pbes p = pbes_system::txt2pbes(pbes_spec);
  pbes_system::detail::parity_game_output pgg(p);
  pgg.run();

// check the result
#ifdef MCRL2_PGSOLVER_ENABLED
  static int index;
  BOOST_CHECK(pbes_system::detail::pbessolve(p,expected_result));
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
void test_pbespgsolve(const std::string& pbes_spec)
{
  pbes_system::pbes p = pbes_system::txt2pbes(pbes_spec);
  pbes_system::parity_game_generator pgg(p, true, true);
  std::size_t begin = 0;
  std::size_t end = 3;
  for (std::size_t v = begin; v < end; ++v)
  {
    std::set<std::size_t> deps = pgg.get_dependencies(v);
    for (std::size_t w: deps)
    {
      assert(w >= begin);
      if (w >= end)
      {
        end = w + 1;
      }
      // edges.push_back(std::make_pair(v - begin, w - begin));
    }

    int max_prio = 0;
    for (std::size_t m = begin; m < end; ++m)
    {
      max_prio = (std::max)(max_prio, (int) pgg.get_priority(m));
    }

    for (std::size_t n = begin; n < end; ++n)
    {
      pgg.get_priority(n);
    }
  }
}

void test_lps(const std::string& lps_spec, const bool expected_result, const std::string& formula = lps::detail::NO_DEADLOCK())
{
  using namespace pbes_system;
  bool timed = false;
  pbes p = lps2pbes(lps_spec, formula, timed);
  std::string text = pbes_system::pp(p);
  test_pbes(text,expected_result);
}

BOOST_AUTO_TEST_CASE(abp_test)
{
  test_lps(lps::detail::ABP_SPECIFICATION(),true);
}

BOOST_AUTO_TEST_CASE(bes_test)
{
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

  test_pbes(BES1, false);
  test_pbes(BES2, true);
  test_pbes(BES3, false);
  test_pbes(BES4, true);
  test_pbes(BES5, true);
  test_pbes(BES6, false);
  test_pbes(BES7, false);
  test_pbes(BES8, true);
}

BOOST_AUTO_TEST_CASE(pbes_test)
{
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

  test_pbes(PBES1, false);
  test_pbes(PBES2, false);
  test_pbes(PBES3, true);
  test_pbespgsolve(PBES1);
  test_pbespgsolve(PBES2);
  test_pbespgsolve(PBES3);
}

#ifdef MCRL2_EXTENDED_TESTS
BOOST_AUTO_TEST_CASE(slow_tests)
{
  test_lps(lps::detail::DINING3_SPECIFICATION(),false);
  test_lps(lps::detail::ONE_BIT_SLIDING_WINDOW_SPECIFICATION(), true, "nu X. <true>true && [true]X");
}
#endif
