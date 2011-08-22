// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file absinthe_test.cpp
/// \brief Test program for absinthe algorithm.

#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/pbes/absinthe.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;

std::string SORT_EXPRESSION_MAPPING =
  "D       := AbsD                                    \n"
  "Nat     := AbsNat                                  \n"
  "List(D) := AbsList                                 \n"
  ;

std::string FUNCTION_SYMBOL_MAPPING =
  "tail: List(D) -> List(D)   := Abstail    : AbsList -> Set(AbsList)        \n"
  "|>: D # List(D) -> List(D) := Absconc    : AbsD # AbsList -> Set(AbsList) \n"
  "#: List(D) -> Nat          := Abslen     : AbsList -> Set(AbsNat)         \n"
  ">=: Nat # Nat -> Bool      := Absge      : AbsNat # AbsNat -> Set(Bool)   \n"
  "capacity: Pos              := Abscapacity: AbsNat                         \n"
  ;

std::string PBESSPEC =
  "sort D;                                            \n"
  "                                                   \n"
  "map  d: D;                                         \n"
  "     capacity: Pos;                                \n"
  "                                                   \n"
  "eqn  capacity >= 2  =  true;                       \n"
  "                                                   \n"
  "pbes nu X(l: List(D)) =                            \n"
  "  X(tail(l))                                       \n"
  "&&                                                 \n"
  "  (forall e: D. val(#l >= capacity) || X(e |> l)); \n"
  "                                                   \n"
  "init X([]);                                        \n"
  ;

std::string DATASPEC =
  "sort                                        \n"
  "  AbsD    = struct a;                       \n"
  "  AbsList = struct empty | one | more;      \n"
  "  AbsNat  = struct nul | een | meer;        \n"
  "                                            \n"
  "eqn                                         \n"
  "  Abstail(empty) = {empty};                 \n"
  "  Abstail(one)   = {empty};                 \n"
  "  Abstail(more)  = {one,more};              \n"
  "                                            \n"
  "  Absconc(a,empty) = {one};                 \n"
  "  Absconc(a,one)   = {more};                \n"
  "  Absconc(a,more)  = {more};                \n"
  "                                            \n"
  "  Abslen(empty)    = {nul};                 \n"
  "  Abslen(one)      = {een};                 \n"
  "  Abslen(more)     = {meer};                \n"
  "                                            \n"
  "  Absge(nul,nul)   = {true};                \n"
  "  Absge(nul,een)   = {false};               \n"
  "  Absge(nul,meer)  = {false};               \n"
  "  Absge(een,nul)   = {true};                \n"
  "  Absge(een,een)   = {true};                \n"
  "  Absge(een,meer)  = {false};               \n"
  "  Absge(meer,nul)  = {true};                \n"
  "  Absge(meer,een)  = {true};                \n"
  "  Absge(meer,meer) = {false,true};          \n"
  "                                            \n"
  "  Abscapacity      = meer;                  \n"
  ;

void test_absinthe()
{
SORT_EXPRESSION_MAPPING =
"D := AbsD         \n"
"Frame := AbsFrame \n"
;

FUNCTION_SYMBOL_MAPPING =
"getack : Frame -> Ack  := Absgetack : AbsFrame -> Set(Ack) \n"
"d0 : D                 := Absd0 : AbsD                     \n"
;

PBESSPEC =
"sort D;                                                                                   \n"
"     Ack = struct ok | nok | error;                                                       \n"
"     Frame = struct f(data: D, acknowledgement: Ack);                                     \n"
"     E = struct e2 | e1 | e0;                                                             \n"
"                                                                                          \n"
"map  d0: D;                                                                               \n"
"     getack: Frame -> Ack;                                                                \n"
"                                                                                          \n"
"var  d: D;                                                                                \n"
"     a: Ack;                                                                              \n"
"eqn  getack(f(d, a))  =  acknowledgement(f(d, a));                                        \n"
"                                                                                          \n"
"pbes                                                                                      \n"
"                                                                                          \n"
"nu X(s: E, dd: D) =                                                                       \n"
"     (forall d: D. forall d': D. (val(!(d' == d)) || val(!(s == e2))) || Y(e1, d', d))    \n"
"  && (forall d: D. val(!(s == e2)) || X(e1, d))                                           \n"
"  && (forall a: Ack. val(!(s == e1 && a == ok)) || X(e0, dd))                             \n"
"  && (forall a: Ack. val(!(s == e1 && !(a == ok))) || X(e1, dd))                          \n"
"  && (val(!(s == e0)) || X(e2, d0));                                                      \n"
"                                                                                          \n"
"mu Y(s: E, dd,d: D) =                                                                     \n"
"       (forall d': D. val(!(s == e2)) || Y(e1, d', d))                                    \n"
"    && (forall a': Ack. val(!(s == e1 && a' == ok)) || Y(e0, dd, d))                      \n"
"    && (forall a': Ack. val(!(s == e1 && !(a' == ok))) || Y(e1, dd, d))                   \n"
"    && ((val(dd == d) || val(!(s == e0))) || Y(e2, d0, d));                               \n"
"                                                                                          \n"
"init X(e2, d0);                                                                           \n"
;

DATASPEC =
"sort                                                   \n"
" AbsD;                                                 \n"
" AbsFrame = struct f(data:AbsD, acknowledgement: Ack); \n"
"                                                       \n"
"var                                                    \n"
"  d : AbsD;                                            \n"
"  a : Ack;                                             \n"
"                                                       \n"
"eqn                                                    \n"
"  Absgetack (f(d,a)) = {acknowledgement(f(d,a))};      \n"
;

  pbes<> p = txt2pbes(PBESSPEC);
  pbes_system::detail::print_used_function_symbols(p);
  absinthe_algorithm algorithm;
  algorithm.run(p, SORT_EXPRESSION_MAPPING, FUNCTION_SYMBOL_MAPPING, DATASPEC, false);
  core::garbage_collect();
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_absinthe();

  return 0;
}
