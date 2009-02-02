// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bes_test3.cpp
/// \brief Test case for pbes2bes that does not seem to terminate, although
/// it is a relatively small system. This needs to be investigated.

#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/pbes2bes_algorithm.h"

using namespace mcrl2;

const std::string BALANCE_PLAT_SPECIFICATION =
  " % Specification of balancing coins to determine the single coin with                 \n"
  " % different weight.                                                                  \n"
  "                                                                                      \n"
  " % C: Total number of coins                                                           \n"
  " map  C: Nat;                                                                         \n"
  " eqn  C = 12;                                                                         \n"
  "                                                                                      \n"
  " % Every coin can be in one of four categories: NHL, NH, NL, and N,                   \n"
  " % where:                                                                             \n"
  " % N: possibly normal weight                                                          \n"
  " % H: possibly heavy weight                                                           \n"
  " % L: possibly light weight                                                           \n"
  " % We count the number of coins in every category, but we do not count                \n"
  " % the number of coins in N explicitly, because:                                      \n"
  " %   |N| = C - ( |NHL| + |NH| + |NL| )                                                \n"
  "                                                                                      \n"
  " map                                                                                  \n"
  "      lexleq: Nat # Nat # Nat # Nat # Nat # Nat -> Bool;                              \n"
  "      is_better: Nat # Nat # Nat # Nat # Nat # Nat -> Bool;                           \n"
  "      is_useful: Nat # Nat # Nat # Nat # Nat # Nat # Nat # Nat # Nat -> Bool;         \n"
  "      is_possible: Nat # Nat # Nat # Nat # Nat # Nat # Nat # Nat # Nat -> Bool;       \n"
  "                                                                                      \n"
  " var  d1,d2,d3,e1,e2,e3,f1,f2,f3: Nat;                                                \n"
  "                                                                                      \n"
  " eqn                                                                                  \n"
  "      % lexicographical ordening on distributions; this is needed to                  \n"
  "      % eliminate half of the possibilities for balancing: only consider              \n"
  "      % X vs. Y and not Y vs. X, if X <= Y.                                           \n"
  "      lexleq(d1,d2,d3,e1,e2,e3) =                                                     \n"
  "          d1<e1 || (d1==e1 && d2<e2) || (d1==e1 && d2==e2 && d3<=e3);                 \n"
  "                                                                                      \n"
  "      % determines whether a distribution d is 'better than' a                        \n"
  "      % distribution e, in the sense that in d we have more certainty (or             \n"
  "      % less uncertainty) about a larger number of coins                              \n"
  "      is_better(d1,d2,d3,e1,e2,e3) = d1+d2+d3 < e1+e2+e3 || d1 < e1;                  \n"
  "                                                                                      \n"
  "      % determines whether weighing e against f is useful in situation d:             \n"
  "      % all possible outcomes should be an improvement                                \n"
  "      is_useful(d1,d2,d3,e1,e2,e3,f1,f2,f3) =                                         \n"
  "        is_better(Int2Nat(d1-e1-f1),Int2Nat(d2-e2-f2),Int2Nat(d3-e3-f3),d1,d2,d3) &&  \n"
  "        is_better(0,e1+e2,f1+f3,d1,d2,d3) &&                                          \n"
  "        is_better(0,f1+f2,e1+e3,d1,d2,d3);                                            \n"
  "                                                                                      \n"
  "      % determines whether weighing e against f is possible in situation              \n"
  "      % d:                                                                            \n"
  "      % - for every category X: X(e) + X(f) <= X(d)                                   \n"
  "      % - if total(e) < total(f) then N 'normal' coins are added to e                 \n"
  "      %   such that N = total(f) - total(e), so N 'normal' coins must                 \n"
  "      %   be available in situation d, i.e. N <= C - total(d).                        \n"
  "      % - analogously if total(e) > total(f).                                         \n"
  "      is_possible(d1,d2,d3,e1,e2,e3,f1,f2,f3) =                                       \n"
  "        e1+f1 <= d1 && e2+f2 <= d2 && e3+f3 <= d3 &&                                  \n"
  "        ( e1+e2+e3 == f1+f2+f3 ||                                                     \n"
  "          (e1+e2+e3 < f1+f2+f3 && f1+f2+f3 - e1-e2-e3 <= C - d1-d2-d3) ||             \n"
  "          (f1+f2+f3 < e1+e2+e3 && e1+e2+e3 - f1-f2-f3 <= C - d1-d2-d3)                \n"
  "        );                                                                            \n"
  "                                                                                      \n"
  " act  weigh, equal, greater, smaller: Nat # Nat # Nat # Nat # Nat # Nat;              \n"
  "      done;                                                                           \n"
  "                                                                                      \n"
  " proc BalancingAct(NHL,NH,NL:Nat) =                                                   \n"
  "      % we're done if |NHL| + |NH| + |NL| == 1                                        \n"
  "        (NHL+NH+NL == 1) -> done . BalancingAct(NHL,NH,NL)                            \n"
  "                                                                                      \n"
  "      + (NHL+NH+NL >  1) ->                                                           \n"
  "          (                                                                           \n"
  "            sum nhl_l,nh_l,nl_l:Nat, nhl_r,nh_r,nl_r:Nat .                            \n"
  "                                                                                      \n"
  "            (lexleq(nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r) &&                               \n"
  "            is_possible(NHL,NH,NL,nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r) &&                 \n"
  "            is_useful(NHL,NH,NL,nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r)) ->                  \n"
  "                                                                                      \n"
  "              weigh(nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r) .                                \n"
  "              (                                                                       \n"
  "              % left and right have equal weight                                      \n"
  "              ((NHL-nhl_l-nhl_r + NH-nh_l-nh_r + NL-nl_l-nl_r > 0) ->                 \n"
  "                equal(nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r) .                              \n"
  "                  BalancingAct(Int2Nat(NHL-nhl_l-nhl_r),                              \n"
  "                               Int2Nat(NH-nh_l-nh_r),                                 \n"
  "                               Int2Nat(NL-nl_l-nl_r)))                                \n"
  "              +                                                                       \n"
  "              % left is heavier than right                                            \n"
  "              ((nhl_l+nh_l + nhl_r+nl_r > 0) ->                                       \n"
  "                greater(nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r) .                            \n"
  "                  BalancingAct(0,nhl_l+nh_l,nhl_r+nl_r))                              \n"
  "              +                                                                       \n"
  "              % left is lighter than right                                            \n"
  "              ((nhl_r+nh_r + nhl_l+nl_l > 0) ->                                       \n"
  "                smaller(nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r) .                            \n"
  "                  BalancingAct(0,nhl_r+nh_r,nhl_l+nl_l))                              \n"
  "              )                                                                       \n"
  "          );                                                                          \n"
  "                                                                                      \n"
  " init BalancingAct(C,0,0);                                                            \n"
  ;  

const std::string NO_DEADLOCK = "[true*]<true>true";

inline
pbes_system::pbes<> pbes2bes(pbes_system::pbes<>& p)
{
  pbes_system::pbes2bes_algorithm algorithm(p.data());
  algorithm.run(p);
  return algorithm.get_result();
}

void test_balancing_plat()
{
  using namespace pbes_system;

  lps::specification spec = lps::mcrl22lps(BALANCE_PLAT_SPECIFICATION);
  modal::state_formula formula = modal::detail::mcf2statefrm(NO_DEADLOCK, spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  pbes<> q = pbes2bes(p);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_balancing_plat();

  return 0;
}
