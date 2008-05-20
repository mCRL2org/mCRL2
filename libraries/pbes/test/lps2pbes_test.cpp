// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2pbes_test.cpp
/// \brief Add your file description here.

// Test program for timed lps2pbes.

#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/core/text_utility.h"
#include "test_specifications.h"

using namespace std;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;
using namespace mcrl2::modal;
using namespace mcrl2::modal::detail;
using namespace mcrl2::pbes_system;
namespace fs = boost::filesystem;

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
}

void test_lps2pbes2()
{
  std::string FORMULA;
  specification spec;
  state_formula formula;
  pbes<> p;
  bool timed = false;

  FORMULA = "nu X. ([true]X && <true>true)";
  p = lps2pbes(ABP_SPECIFICATION, FORMULA, timed);
  BOOST_CHECK(p.is_well_typed());

  FORMULA = "nu X. ([true]X && forall d:D. [r1(d)] mu Y. (<true>Y || <s4(d)>true))";
  p = lps2pbes(ABP_SPECIFICATION, FORMULA, timed);
  BOOST_CHECK(p.is_well_typed());

  FORMULA = "forall d:D. nu X. (([!r1(d)]X && [s4(d)]false))";
  p = lps2pbes(ABP_SPECIFICATION, FORMULA, timed);
  BOOST_CHECK(p.is_well_typed());

  FORMULA = "nu X. ([true]X && forall d:D. [r1(d)]nu Y. ([!r1(d) && !s4(d)]Y && [r1(d)]false))";
  p = lps2pbes(ABP_SPECIFICATION, FORMULA, timed);
  BOOST_CHECK(p.is_well_typed());
}

void test_directory(int argc, char** argv)
{
  BOOST_CHECK(argc > 1);

  // The dummy file test.test is used to extract the full path of the test directory.
  fs::path dummy_path = fs::system_complete( fs::path( argv[1], fs::native ) );
  fs::path dir = dummy_path.branch_path();
  BOOST_CHECK(fs::is_directory(dir));

  fs::directory_iterator end_iter;
  for ( fs::directory_iterator dir_itr(dir); dir_itr != end_iter; ++dir_itr )
  {
    if ( fs::is_regular( dir_itr->status() ) )
    {
      std::string filename = dir_itr->path().file_string();
      if (boost::ends_with(filename, std::string(".form")))
      {
        std::string timed_result_file   = filename.substr(0, filename.find_last_of('.') + 1) + "expected_timed_result";
        std::string untimed_result_file = filename.substr(0, filename.find_last_of('.') + 1) + "expected_untimed_result";
        std::string formula = core::read_text(filename);
        if (fs::exists(timed_result_file))
        {
          try {
            pbes<> result = lps2pbes(SPEC1, formula, true);
            pbes<> expected_result;
            expected_result.load(timed_result_file);
            bool cmp = (result == expected_result);
            if (!cmp)
              cerr << "ERROR: test " << timed_result_file << " failed!" << endl;
            BOOST_CHECK(cmp);
          }
          catch (std::runtime_error e)
          {
            cerr << e.what() << endl;
          }
        }
        if (fs::exists(untimed_result_file))
        {
          try {
            pbes<> result = lps2pbes(SPEC1, formula, false);
            BOOST_CHECK(result.is_well_typed());
            pbes<> expected_result;
            expected_result.load(untimed_result_file);
            bool cmp = (result == expected_result);
            if (!cmp)
              cerr << "ERROR: test " << untimed_result_file << " failed!" << endl;
            BOOST_CHECK(cmp);
          }
          catch (std::runtime_error e)
          {
            cerr << e.what() << endl;
          }
        }
      }
    }
  }
}

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
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_lps2pbes();
  test_lps2pbes2();
  test_trivial();
  test_formulas();
  //test_directory();

  return 0;
}
