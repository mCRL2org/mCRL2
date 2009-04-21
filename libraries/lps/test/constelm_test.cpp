// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file constelm_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "mcrl2/core/text_utility.h"
#include "mcrl2/new_data/rewriter.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/lps/constelm.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/linear_process_expression_visitor.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::new_data;
using namespace mcrl2::lps;

std::string case_1 =
  "% Test Case 1 -- No Free Variables                                               \n"
  "%                                                                                \n"
  "% Process parameter i is substituted by 0 and removed from the list of process   \n"
  "% parameters.                                                                    \n"
  "                                                                                 \n"
  "act action :Nat;                                                                 \n"
  "                                                                                 \n"
  "proc P(i: Nat) = action(i). P(i);                                                \n"
  "                                                                                 \n"
  "init P(0);                                                                       \n"
  ;
const std::string removed_1 = "i";

std::string case_2 =
  "% Test Case 2 -- No Free Variables                                               \n"
  "%                                                                                \n"
  "% Process parameter i will not be removed.                                       \n"
  "                                                                                 \n"
  "act action :Nat;                                                                 \n"
  "                                                                                 \n"
  "proc P(i: Nat) = action(i). P(i+1);                                              \n"
  "                                                                                 \n"
  "init P(0);                                                                       \n"
  ;
const std::string removed_2 = "";

std::string case_3 =
  "% Test Case 3 -- No Free Variables                                               \n"
  "%                                                                                \n"
  "% Process parameter i will not be removed. Occurrences of process parameter j are\n"
  "% substituted by 5 and removed from the process parameter list.                  \n"
  "                                                                                 \n"
  "act action :Nat;                                                                 \n"
  "                                                                                 \n"
  "proc P(i, j: Nat) = action(j). P(i+1, j);                                        \n"
  "                                                                                 \n"
  "init P(0,5);                                                                     \n"
  ;
const std::string removed_3 = "j";

std::string case_4 =
  "% Test Case 4 -- No Free Variables                                               \n"
  "%                                                                                \n"
  "% Process parameter i will not be removed. Occurrences of process parameter j are\n"
  "% subtituted by 5 and removed from the process parameter list.                   \n"
  "%                                                                                \n"
  "% If --nocondition is used, occurrences of process parameters j are NOT          \n"
  "% substituted and removed. If --noreachable are summand                          \n"
  "% \"false -> action(j). P(i+1,j+1);\" will not be removed. If both options are   \n"
  "% used, the LPS remains the same.                                                \n"
  "                                                                                 \n"
  "act action :Nat;                                                                 \n"
  "                                                                                 \n"
  "proc P(i, j: Nat) = true  -> action(j). P(i+1,j)   +                             \n"
  "                    false -> action(j). P(i+1,j+1);                              \n"
  "                                                                                 \n"
  "init P(0,5);                                                                     \n"
  "                                                                                 \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
  "%% NOTE:                                                  %%                     \n"
  "%% =====                                                  %%                     \n"
  "%%                                                        %%                     \n"
  "%% Use: mcrl22lps --no-cluster $DIR$/case4.mcrl2          %%                     \n"
  "%%                                                        %%                     \n"
  "%% Not using \"no-cluster\" will result in differt results. %%                   \n"
  "%%                                                        %%                     \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
  ;
const std::string removed_4 = "j";

std::string case_5 =
  "% Test Case 5 -- No Free Variables                                               \n"
  "%                                                                                \n"
  "% Process parameter i will not be removed. Occurrences of process parameter j are\n"
  "% substituted by \"x\" and removed from the process parameter list.              \n"
  "%                                                                                \n"
  "% If \"--nosingleton\" is used, process parameter j are not substituted and      \n"
  "% not removed from the process parameter list.                                   \n"
  "                                                                                 \n"
  "sort Singleton = struct x;                                                       \n"
  "                                                                                 \n"
  "act  action :Nat;                                                                \n"
  "                                                                                 \n"
  "proc P(i : Nat, j : Singleton ) = true -> action(i). P(i+1,j);                   \n"
  "                                                                                 \n"
  "init P(0,x);                                                                     \n"
  ;
const std::string removed_5 = "j";

std::string case_6 =
  "% Test Case 6 -- No Free Variables                                               \n"
  "%                                                                                \n"
  "% Process parameter i is substituted by 0 and removed from the list of process   \n"
  "% parameters.                                                                    \n"
  "                                                                                 \n"
  "act action :Nat;                                                                 \n"
  "                                                                                 \n"
  "proc P(i: Nat) = action(i). Q(i);                                                \n"
  "     Q(i: Nat) = action(i). P(i);                                                \n"
  "                                                                                 \n"
  "init P(0);                                                                       \n"
  "                                                                                 \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
  "%% NOTE:                                                  %%                     \n"
  "%% =====                                                  %%                     \n"
  "%%                                                        %%                     \n"
  "%% Usage of: mcrl22lps $DIR$/case6.mcrl2                  %%                     \n"
  "%% + indicates the second process parameter is constant   %%                     \n"
  "%% Usage of: mcrl22lps --no-cluster $DIR$/case6.mcrl2     %%                     \n"
  "%% + indicates the second process parameter is constant   %%                     \n"
  "%% In this case they are the same                         %%                     \n"
  "%%                                                        %%                     \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
  ;
const std::string removed_6 = "i";

std::string case_7 =
  "% Test Case 7 -- Free Variables                                                  \n"
  "%                                                                                \n"
  "% Process parameters s3, j and i are marked constant and all occurrences are     \n"
  "% substituted.                                                                   \n"
  "%                                                                                \n"
  "% If \"--no-condition\" is used only j is marked constant and all occurrences of \n"
  "% are substituted.                                                               \n"
  "                                                                                 \n"
  "act action :Nat;                                                                 \n"
  "                                                                                 \n"
  "proc P(i,j: Nat) = (i > 5) -> action(i). P(i+1,j) +                              \n"
  "                   (i == 5) -> action(j). Q(j);                                  \n"
  "     Q(i: Nat)   = action(i). Q(i);                                              \n"
  "                                                                                 \n"
  "init P(0,0);                                                                     \n"
  "                                                                                 \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
  "%% NOTE:                                                  %%                     \n"
  "%% =====                                                  %%                     \n"
  "%%                                                        %%                     \n"
  "%% Use: mcrl22lps --no-cluster $DIR$/case7.mcrl2          %%                     \n"
  "%%                                                        %%                     \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
  "                                                                                 \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
  "%%                                                        %%                     \n"
  "%% var  freevar,freevar0: Nat;                            %%                     \n"
  "%% proc P(s3: Pos, j,i: Nat) =                            %%                     \n"
  "%%        (s3 == 2) ->                                    %%                     \n"
  "%%          action(i) .                                   %%                     \n"
  "%%          P(s3 := 2, j := freevar0)                     %%                     \n"
  "%%      + (s3 == 1 && 5 < i ) ->                          %%                     \n"
  "%%          action(i) .                                   %%                     \n"
  "%%          P(s3 := 1, i := i + 1)                        %%                     \n"
  "%%      + (s3 == 1 && i == 5) ->                          %%                     \n"
  "%%          action(j) .                                   %%                     \n"
  "%%          P(s3 := 2, j := freevar, i := j);             %%                     \n"
  "%%                                                        %%                     \n"
  "%% init P(s3 := 1, j := 0, i := 0);                       %%                     \n"
  "%%                                                        %%                     \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
  ;

std::string case_8 =
  "% Test Case 8 -- Free Variables                                                  \n"
  "%                                                                                \n"
  "% No constant parameters are found                                               \n"
  "%                                                                                \n"
  "% lpsconstelm cannot detect (i==5)                                               \n"
  "                                                                                 \n"
  "act action: Nat;                                                                 \n"
  "                                                                                 \n"
  "proc X(i: Nat)   = (i <  5) -> action(i).X(i+1) +                                \n"
  "                   (i == 5) -> action(i).Y(i, i);                                \n"
  "     Y(i,j: Nat) = action(j).Y(i,j);                                             \n"
  "                                                                                 \n"
  "init X(0);                                                                       \n"
  "                                                                                 \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
  "%% NOTE:                                                  %%                     \n"
  "%% =====                                                  %%                     \n"
  "%%                                                        %%                     \n"
  "%% Use: mcrl22lps --no-cluster $DIR$/case8.mcrl2          %%                     \n"
  "%%                                                        %%                     \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
  "                                                                                 \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
  "%%                                                        %%                     \n"
  "%% act  action: Nat;                                      %%                     \n"
  "%%                                                        %%                     \n"
  "%% var  freevar0: Nat;                                    %%                     \n"
  "%% proc P(s3: Pos, i,j: Nat) =                            %%                     \n"
  "%%       (s3 == 2) ->                                     %%                     \n"
  "%%         action(j) .                                    %%                     \n"
  "%%         P(s3 := 2)                                     %%                     \n"
  "%%     + (s3 == 1 && i < 5) ->                            %%                     \n"
  "%%         action(i) .                                    %%                     \n"
  "%%         P(s3 := 1, i := i + 1, j := freevar0)          %%                     \n"
  "%%     + (s3 == 1 && i == 5) ->                           %%                     \n"
  "%%         action(i) .                                    %%                     \n"
  "%%         P(s3 := 2, j := i);                            %%                     \n"
  "%%                                                        %%                     \n"
  "%% var  freevar: Nat;                                     %%                     \n"
  "%% init P(s3 := 1, i := 0, j := freevar);                 %%                     \n"
  "%%                                                        %%                     \n"
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
  ;

inline
bool is_linear(const process_specification& pspec)
{
  return lps::detail::linear_process_expression_visitor().is_linear(pspec);
}

void test_constelm(const std::string& spec_text, const std::string& expected_result)
{
  specification s0;
  process_specification pspec = parse_process_specification(spec_text);
  if (is_linear(pspec))
  {
    s0 = parse_linear_process_specification(spec_text);
  }
  else
  {
    s0 = mcrl22lps(spec_text);
  }
  new_data::rewriter datar(s0.data());
  specification s1 = constelm(s0, datar, true);
  variable_list v0 = s0.process().process_parameters();
  variable_list v1 = s1.process().process_parameters();

  // create a set of strings set1 that contains the names of expected removed parameters
  std::vector<std::string> removed = core::regex_split(expected_result, "\\s");
  std::set<std::string> set1;
  set1.insert(removed.begin(), removed.end());

  // create a set of strings set2 that contains the names of actually removed parameters
  std::set<std::string> set2;
  for (variable_list::const_iterator i = v0.begin(); i != v0.end(); i++)
  {
    if (std::find(v1.begin(), v1.end(), *i) == v1.end())
    {
      set2.insert(i->name());
    }
  }

  // check if the parelm result is correct
  if (set1 != set2)
  {
    std::cerr << "--- failed test ---\n";
    std::cerr << spec_text << std::endl;
    std::cerr << "expected result " << boost::algorithm::join(set1, " ") << std::endl;
    std::cerr << "computed result " << boost::algorithm::join(set2, " ") << std::endl;
  }
  BOOST_CHECK(set1 == set2);

  // check if the number of free variables is unchanged
  BOOST_CHECK(s0.process().free_variables().size() == s1.process().free_variables().size());
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_constelm(case_1, removed_1);
  core::garbage_collect();
  test_constelm(case_2, removed_2);
  core::garbage_collect();
  test_constelm(case_3, removed_3);
  core::garbage_collect();
  test_constelm(case_4, removed_4);
  core::garbage_collect();
  test_constelm(case_5, removed_5);
  core::garbage_collect();
  // test_constelm(case_6, removed_6);

  return 0;
}
