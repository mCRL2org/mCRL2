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
//#define MCRL2_PARITY_GAME_CREATE_FILES
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
//#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <fstream>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/detail/parity_game_output.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

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

void test_bes(std::string bes_spec, std::string output_file, bool expected_result)
{
  pbes_system::pbes<> p = pbes_system::txt2pbes(bes_spec);
  pbes_system::detail::parity_game_output pgg(p);
  pgg.run();
  {
    std::string text = pgg.python_graph();
#ifdef MCRL2_PARITY_GAME_CREATE_FILES
    std::ofstream to(output_file.c_str());
    to << text << std::endl;
#endif
  }
  {
    std::string text = pgg.pgsolver_graph();
#ifdef MCRL2_PARITY_GAME_CREATE_FILES
    std::ofstream to((output_file + "1").c_str());
    to << text << std::endl;
#endif
  }
  core::garbage_collect();
}

std::string PBES1 =
  "pbes                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \n"
  "nu X0 = ((!(forall m:Nat.((val(m < 3)) && (!X2(m + 1, 0))))) || (((forall k:Nat.((val(k < 3)) && (val(k < 2)))) || (exists n:Nat.((val(n < 3)) || (val(n < 2))))) || ((val(true)) => (exists k:Nat.((val(k < 3)) || (exists m:Nat.((val(m < 3)) || (forall k:Nat.((val(k < 3)) && (X4(1, m + 1))))))))))) && ((!(exists n:Nat.((val(n < 3)) || (exists k:Nat.((val(k < 3)) || (val(false))))))) || (forall n:Nat.((val(n < 3)) && (exists n:Nat.((val(n < 3)) || (forall m:Nat.((val(m < 3)) && (exists k:Nat.((val(k < 3)) || (X3)))))))))); \n"
  "mu X1(b:Bool) = ((exists m:Nat.((val(m < 3)) || (val(b)))) => ((val(false)) || (X3))) && (forall k:Nat.((val(k < 3)) && ((((val(k > 1)) => (X1(k > 1))) && (val(false))) && (exists k:Nat.((val(k < 3)) || (X4(k + 1, k + 1)))))));                                                                                                                                                                                                                                                                                                           \n"
  "mu X2(n:Nat, m:Nat) = ((exists k:Nat.((val(k < 3)) || (!(val(n < 2))))) && ((X3) || ((forall k:Nat.((val(k < 3)) && (!((val(m < 3)) || (val(m == k)))))) || (!((X0) => (!X4(n + 1, 1))))))) && (exists n:Nat.((val(n < 3)) || (forall m:Nat.((val(m < 3)) && (exists k:Nat.((val(k < 3)) || (val(k < 3))))))));                                                                                                                                                                                                                               \n"
  "mu X3 = ((val(true)) || (X3)) || (!((forall n:Nat.((val(n < 3)) && (!((val(true)) || ((!(val(n > 1))) && ((X1(n < 3)) || (val(n > 1)))))))) && (forall n:Nat.((val(n < 3)) && (exists m:Nat.((val(m < 3)) || (!X0)))))));                                                                                                                                                                                                                                                                                                                     \n"
  "mu X4(n:Nat, m:Nat) = ((((forall m:Nat.((val(m < 3)) && ((val(m < 2)) || (val(n < 2))))) => (X1(true))) || (forall m:Nat.((val(m < 3)) && (exists m:Nat.((val(m < 3)) || (val(n > 0))))))) || (forall n:Nat.((val(n < 3)) && ((!(forall n:Nat.((val(n < 3)) && (!X4(0, 1))))) || (forall k:Nat.((val(k < 3)) && (val(true)))))))) || (exists m:Nat.((val(m < 3)) || (forall n:Nat.((val(n < 3)) && (!(!X0))))));                                                                                                                              \n"
  "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \n"
  "init X0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \n"
  ;

// mimick the way parity_game_generator is used in parity game solver from Twente
void test_pbes(std::string pbes_spec)
{
  pbes_system::pbes<> p = pbes_system::txt2pbes(pbes_spec);
  pbes_system::parity_game_generator pgg(p, true, true);
  unsigned int begin = 0;
  unsigned int end = 3;
  for (unsigned int v = begin; v < end; ++v)
  {
    std::set<unsigned> deps = pgg.get_dependencies(v);
    for (std::set<unsigned>::const_iterator it = deps.begin(); it != deps.end(); ++it )
    {
      unsigned int w = *it;
      assert(w >= begin);
      if (w >= end)
      {
      	end = w + 1;
      }
      // edges.push_back(std::make_pair(v - begin, w - begin));
    }

    int max_prio = 0;                                                        
    for (unsigned int v = begin; v < end; ++v)                                      
    {                                                                        
      max_prio = std::max(max_prio, (int)pgg.get_priority(v));             
    }                                                                        
                                                                             
    for (unsigned int v = begin; v < end; ++v)                                      
    {                                                                        
      bool and_op = pgg.get_operation(v) == mcrl2::pbes_system::parity_game_generator::PGAME_AND;
      pgg.get_priority(v);                   
    }                                                                        
  }
}

void test_one_bit_sliding_window()
{
  std::string spec_text =
    "% This file describes the onebit sliding window protocol as documented     \n"
    "% M.A. Bezem and J.F. Groote. A correctness proof of a one bit sliding     \n"
    "% window protocol in muCRL. The Computer Journal, 37(4): 289-307, 1994.    \n"
    "% This sliding window protocol is a bidirectional sliding window protocol  \n"
    "% with piggy backing, where the window sizes at the receiving and          \n"
    "% sending side have size 1. The behaviour of this sliding window protocol  \n"
    "% is that of two bidirectional buffers sending data from channel ra to     \n"
    "% sb, and from rc to sd. Both buffers have capacity 2.                     \n"
    "%   The external behaviour of the onebit protocol is rather complex.       \n"
    "% However, making only the behaviour visible at gates ra and sb reduced    \n"
    "% modulo branching bisimulation clearly shows the behaviour of             \n"
    "% a buffer of capacity 2.                                                  \n"
    "%                                                                          \n"
    "% Jan Friso Groote, translated from muCRL 30/12/2006                       \n"
    "                                                                           \n"
    "sort Bit = struct e0 | e1;                                                 \n"
    "     D= struct d1;                                                         \n"
    "     Frame=struct frame(dat:D,bit1:Bit,bit2:Bit);                          \n"
    "     Status=struct read?eq_read | choice?eq_choice | del?eq_del;           \n"
    "                                                                           \n"
    "map inv:Bit-> Bit;                                                         \n"
    "eqn  inv(e0)=e1;                                                           \n"
    "     inv(e1)=e0;                                                           \n"
    "                                                                           \n"
    "act  r,w,rc,sd:D;                                                          \n"
    "     rcl,scl,i_del,i_lost,ccl;                                             \n"
    "     r1,s1,c1,s2,r2,c2,s4,r4,c4:Frame;                                     \n"
    "                                                                           \n"
    "proc S(ready:Bool,rec:Bool,sts:Bool,d:D,e:D,p:Bit,q:Bit)=                  \n"
    "       ready -> sum d:D.r(d).S(false,rec,false,d,e,inv(p),q) +             \n"
    "       !rec -> w(e).S(ready,true,sts,d,e,p,q) +                            \n"
    "       rcl.S(ready,rec,false,d,e,p,q)+                                     \n"
    "       sum f:D,b1:Bit,b2:Bit.                                              \n"
    "           r4(frame(f,b1,b2)).                                             \n"
    "              (rec && b1==inv(q)) -> S(b2==p,false,sts,d,f,p,inv(q))       \n"
    "                                  <> S(b2==p,rec,sts,d,e,p,q) +            \n"
    "       !sts -> s1(frame(d,p,q)).S(ready,rec,true,d,e,p,q) +                \n"
    "       delta;                                                              \n"
    "                                                                           \n"
    "proc Tim= scl.Tim;                                                         \n"
    "                                                                           \n"
    "proc C(f:Frame,st:Status)=                                                 \n"
    "       eq_read(st) -> sum f:Frame.r1(f).C(f,choice)<>delta+                \n"
    "       eq_choice(st) -> (i_del.C(f,del)+i_lost.C(f,read))<>delta+          \n"
    "       eq_del(st) -> s2(f).C(f,read)<>delta ;                              \n"
    "                                                                           \n"
    "init hide ({c4,c2,ccl,c1,i_del,i_lost},                                    \n"
    "       allow({c1,ccl,c2,c4,i_del,i_lost,r,w,rc,sd},                        \n"
    "         comm({r2|s2->c2,r4|s4->c4},                                       \n"
    "           rename({w->sd},                                                 \n"
    "             allow({c1,ccl,r,w,s2,r4,i_del,i_lost},                        \n"
    "               comm({rcl|scl->ccl,r1|s1->c1},                              \n"
    "                 S(true,true,true,d1,d1,e0,e0)||                           \n"
    "                 Tim||                                                     \n"
    "                 C(frame(d1,e0,e0),read))))||                              \n"
    "           rename({r->rc,s2->s4,r4->r2},                                   \n"
    "             allow({c1,ccl,r,w,s2,r4,i_del,i_lost},                        \n"
    "               comm({rcl|scl->ccl,r1|s1->c1},                              \n"
    "                 S(true,true,true,d1,d1,e0,e0)||                           \n"
    "                 Tim||                                                     \n"
    "                 C(frame(d1,e0,e0) ,read)))))));                           \n"
    ;

  std::string formula_text = "nu X. <true>true && [true]X";
  bool timed = false;
  pbes_system::pbes<> p = pbes_system::lps2pbes(spec_text, formula_text, timed);
  pbes_system::detail::parity_game_output pgg(p);
  pgg.run();
}

void test_bes_examples()
{
  test_bes(BES1, "parity_game_test_bes1.pg", false);
  test_bes(BES2, "parity_game_test_bes2.pg", true);
  test_bes(BES3, "parity_game_test_bes3.pg", true);
  test_bes(BES4, "parity_game_test_bes4.pg", true);
  test_bes(BES5, "parity_game_test_bes5.pg", false);
  test_bes(BES6, "parity_game_test_bes6.pg", true);
  test_bes(BES7, "parity_game_test_bes7.pg", false);
  test_bes(BES8, "parity_game_test_bes8.pg", true);
  test_bes(PBES1, "parity_game_test_bes8.pg", true);
}

void test_pbes_examples()
{
  test_pbes(BES1 );
  test_pbes(BES2 );
  test_pbes(BES3 );
  test_pbes(BES4 );
  test_pbes(BES5 );
  test_pbes(BES6 );
  test_pbes(BES7 );
  test_pbes(BES8 );
  test_pbes(PBES1);
}

void test_abp()
{
  using namespace pbes_system;

  std::string ABP =
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

  std::string NODEADLOCK = "[true*]<true>true";

  bool timed = false;
  pbes<> pbes = lps2pbes(ABP, NODEADLOCK, timed);

  // Generate min-priority parity game
  mcrl2::pbes_system::parity_game_generator pgg(pbes, true, true);

  // Build the edge list
  unsigned int num_vertices = 1 + *pgg.get_initial_values().rbegin();
  for (unsigned int v = 0; v < num_vertices; ++v)
  {
      std::set<unsigned> deps = pgg.get_dependencies(v);
      for ( std::set<unsigned>::const_iterator it = deps.begin();
            it != deps.end(); ++it )
      {
          unsigned int w = (unsigned int)*it;
          if (w >= num_vertices) num_vertices = w + 1;
          printf("%6d -> %6d\n", v, w);
      }
  }

  // Find vertex properties
  for (unsigned int v = 0; v < num_vertices; ++v)
  {
      bool and_op = pgg.get_operation(v) ==
                    mcrl2::pbes_system::parity_game_generator::PGAME_AND;
      int priority = pgg.get_priority(v);
      printf("%6d: player=%d priority=%d\n", v, and_op, priority);
  }
  core::garbage_collect();
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_bes_examples();
  // test_pbes_examples();
  // test_one_bit_sliding_window();
  test_abp();

  return 0;
}
