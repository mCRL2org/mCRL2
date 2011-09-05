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
#include <boost/algorithm/string/join.hpp>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/pbes/absinthe.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;
using namespace mcrl2::log;

void test_separate_sort_declarations()
{
  std::string DATASPEC =
    "eqn haha; sort                              \n"
    "  AbsD    = struct a;                       \n"
    "  AbsList = struct empty | one | more;      \n"
    "  AbsNat  = struct sort1 | sort2 | meer;    \n"
    "                                            \n"
    "eqn                                         \n"
    "  Abstail(empty) = {empty};                 \n"
    "  Abstail(one)   = {empty};                 \n"
    "  Abstail(more)  = {one,more};              \n"
    "                                            \n"
    "  Absconc(a,empty) = {one}; sort X; eqn     \n"
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

  std::pair<std::string, std::string> result = pbes_system::detail::separate_sort_declarations(DATASPEC);
  std::cout << result.first << "-----------------------------\n" << result.second << std::endl;
}

void test_absinthe(const std::string& pbes_text, const std::string& abstraction_text, bool is_over_approximation)
{
  pbes<> p = txt2pbes(pbes_text);
  absinthe_algorithm algorithm;
  algorithm.run(p, abstraction_text, is_over_approximation);
  core::garbage_collect();
}

void test1()
{
  std::string PBES_TEXT =
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

  std::string ABSTRACTION_TEXT =
  "sort                                                                \n"
  "  AbsD;                                                             \n"
  "  AbsFrame = struct f(data:AbsD, acknowledgement: Ack);             \n"
  "                                                                    \n"
  "var                                                                 \n"
  "  d : AbsD;                                                         \n"
  "  a : Ack;                                                          \n"
  "                                                                    \n"
  "eqn                                                                 \n"
  "  Absgetack (f(d,a)) = {acknowledgement(f(d,a))};                   \n"
  "                                                                    \n"
  "abssort                                                             \n"
  "  D := AbsD                                                         \n"
  "  Frame := AbsFrame                                                 \n"
  "                                                                    \n"
  "absfunc                                                             \n"
  "  getack : Frame -> Ack  := Absgetack : AbsFrame -> Set(Ack)        \n"
  "  d0 : D                 := Absd0     : AbsD                        \n"
  "  ==: D # D -> Bool      := Abseq     : AbsD # AbsD -> Set(Bool)    \n"
  ;

  test_absinthe(PBES_TEXT, ABSTRACTION_TEXT, true);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_separate_sort_declarations();
  test1();
  //BOOST_CHECK(false);

  return 0;
}
