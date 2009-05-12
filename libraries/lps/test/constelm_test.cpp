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
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/lps/constelm.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/process/detail/linear_process_expression_visitor.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::data;
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

// examples/games/domineering.mcrl2
std::string case_9 =
  "sort Position = struct Full | Empty;\n"
  "Row = List(Position);\n"
  "Board = List(List(Position));\n"
  "\n"
  "map  Put: Position # Pos # List(Position) -> List(Position);\n"
  "Put: Position # Pos # Pos # List(List(Position)) -> List(List(Position));\n"
  "At: Pos # Pos # List(List(Position)) -> Position;\n"
  "At: Pos # List(Position) -> Position;\n"
  "N,M: Pos;\n"
  "NoHorizontalSpot,NoVerticalSpot: Pos # Pos # List(List(Position)) -> Bool;\n"
  "\n"
  "var  b: List(List(Position));\n"
  "r: List(Position);\n"
  "p,p': Position;\n"
  "x,y: Pos;\n"
  "eqn  N  =  5;\n"
  "M  =  5;\n"
  "At(x, y, [])  =  Empty;\n"
  "y == 1  ->  At(x, y, r |> b)  =  At(x, r);\n"
  "y > 1  ->  At(x, y, r |> b)  =  At(x, Int2Pos(y - 1), b);\n"
  "At(y, [])  =  Empty;\n"
  "x == 1  ->  At(x, p |> r)  =  p;\n"
  "x > 1  ->  At(x, p |> r)  =  At(Int2Pos(x - 1), r);\n"
  "y == 1  ->  Put(p, x, y, r |> b)  =  Put(p, x, r) |> b;\n"
  "y > 1  ->  Put(p, x, y, r |> b)  =  r |> Put(p, x, Int2Pos(y - 1), b);\n"
  "x == 1  ->  Put(p, x, p' |> r)  =  p |> r;\n"
  "x > 1  ->  Put(p, x, p' |> r)  =  p' |> Put(p, Int2Pos(x - 1), r);\n"
  "y == M  ->  NoVerticalSpot(x, y, b)  =  true;\n"
  "x < N && y < M  ->  NoVerticalSpot(x, y, b)  =  (At(x, y, b) == Full || At(x, y + 1, b) == Full) && NoVerticalSpot(x + 1, y, b);\n"
  "x == N && y < M  ->  NoVerticalSpot(x, y, b)  =  (At(x, y, b) == Full || At(x, y + 1, b) == Full) && NoVerticalSpot(1, y + 1, b);\n"
  "x == N  ->  NoHorizontalSpot(x, y, b)  =  true;\n"
  "x < N && y < M  ->  NoHorizontalSpot(x, y, b)  =  (At(x, y, b) == Full || At(x + 1, y, b) == Full) && NoHorizontalSpot(x, y + 1, b);\n"
  "x < N && y == M  ->  NoHorizontalSpot(x, y, b)  =  (At(x, y, b) == Full || At(x + 1, y, b) == Full) && NoHorizontalSpot(x + 1, 1, b);\n"
  "\n"
  "act  Player1,Player2: Pos # Pos # Pos # Pos;\n"
  "Player1Wins,Player2Wins;\n"
  "\n"
  "proc P(b_Domineering: List(List(struct Full | Empty)), Player1Moves_Domineering: Bool) =\n"
  "(Player1Moves_Domineering && (At(1, 1, b_Domineering) == Full || At(1, 2, b_Domineering) == Full) && (At(2, 1, b_Domineering) == Full || At(2, 2, b_Domineering) == Full) && (At(3, 1, b_Domineering) == Full || At(3, 2, b_Domineering) == Full) && (At(4, 1, b_Domineering) == Full || At(4, 2, b_Domineering) == Full) && (At(5, 1, b_Domineering) == Full || At(5, 2, b_Domineering) == Full) && (At(1, 2, b_Domineering) == Full || At(1, 3, b_Domineering) == Full) && (At(2, 2, b_Domineering) == Full || At(2, 3, b_Domineering) == Full) && (At(3, 2, b_Domineering) == Full || At(3, 3, b_Domineering) == Full) && (At(4, 2, b_Domineering) == Full || At(4, 3, b_Domineering) == Full) && (At(5, 2, b_Domineering) == Full || At(5, 3, b_Domineering) == Full) && (At(1, 3, b_Domineering) == Full || At(1, 4, b_Domineering) == Full) && (At(2, 3, b_Domineering) == Full || At(2, 4, b_Domineering) == Full) && (At(3, 3, b_Domineering) == Full || At(3, 4, b_Domineering) == Full) && (At(4, 3, b_Domineering) == Full || At(4, 4, b_Domineering) == Full) && (At(5, 3, b_Domineering) == Full || At(5, 4, b_Domineering) == Full) && (At(1, 4, b_Domineering) == Full || At(1, 5, b_Domineering) == Full) && (At(2, 4, b_Domineering) == Full || At(2, 5, b_Domineering) == Full) && (At(3, 4, b_Domineering) == Full || At(3, 5, b_Domineering) == Full) && (At(4, 4, b_Domineering) == Full || At(4, 5, b_Domineering) == Full) && At(5, 4, b_Domineering) == Full || At(5, 5, b_Domineering) == Full) ->\n"
  "Player2Wins .\n"
  "P(b_Domineering, Player1Moves_Domineering)\n"
  "+ sum x_Domineering1,y_Domineering1: Pos.\n"
  "(!Player1Moves_Domineering && x_Domineering1 < 5 && y_Domineering1 <= 5 && At(x_Domineering1, y_Domineering1, b_Domineering) == Empty && At(succ(x_Domineering1), y_Domineering1, b_Domineering) == Empty) ->\n"
  "Player2(x_Domineering1, y_Domineering1, x_Domineering1 + 1, y_Domineering1) .\n"
  "P(Put(Full, succ(x_Domineering1), y_Domineering1, Put(Full, x_Domineering1, y_Domineering1, b_Domineering)), true)\n"
  "+ (!Player1Moves_Domineering && (At(1, 1, b_Domineering) == Full || At(2, 1, b_Domineering) == Full) && (At(1, 2, b_Domineering) == Full || At(2, 2, b_Domineering) == Full) && (At(1, 3, b_Domineering) == Full || At(2, 3, b_Domineering) == Full) && (At(1, 4, b_Domineering) == Full || At(2, 4, b_Domineering) == Full) && (At(1, 5, b_Domineering) == Full || At(2, 5, b_Domineering) == Full) && (At(2, 1, b_Domineering) == Full || At(3, 1, b_Domineering) == Full) && (At(2, 2, b_Domineering) == Full || At(3, 2, b_Domineering) == Full) && (At(2, 3, b_Domineering) == Full || At(3, 3, b_Domineering) == Full) && (At(2, 4, b_Domineering) == Full || At(3, 4, b_Domineering) == Full) && (At(2, 5, b_Domineering) == Full || At(3, 5, b_Domineering) == Full) && (At(3, 1, b_Domineering) == Full || At(4, 1, b_Domineering) == Full) && (At(3, 2, b_Domineering) == Full || At(4, 2, b_Domineering) == Full) && (At(3, 3, b_Domineering) == Full || At(4, 3, b_Domineering) == Full) && (At(3, 4, b_Domineering) == Full || At(4, 4, b_Domineering) == Full) && (At(3, 5, b_Domineering) == Full || At(4, 5, b_Domineering) == Full) && (At(4, 1, b_Domineering) == Full || At(5, 1, b_Domineering) == Full) && (At(4, 2, b_Domineering) == Full || At(5, 2, b_Domineering) == Full) && (At(4, 3, b_Domineering) == Full || At(5, 3, b_Domineering) == Full) && (At(4, 4, b_Domineering) == Full || At(5, 4, b_Domineering) == Full) && At(4, 5, b_Domineering) == Full || At(5, 5, b_Domineering) == Full) ->\n"
  "Player1Wins .\n"
  "P(b_Domineering, Player1Moves_Domineering)\n"
  "+ sum x_Domineering,y_Domineering: Pos.\n"
  "(Player1Moves_Domineering && x_Domineering <= 5 && y_Domineering < 5 && At(x_Domineering, y_Domineering, b_Domineering) == Empty && At(x_Domineering, succ(y_Domineering), b_Domineering) == Empty) ->\n"
  "Player1(x_Domineering, y_Domineering, x_Domineering, y_Domineering + 1) .\n"
  "P(Put(Full, x_Domineering, succ(y_Domineering), Put(Full, x_Domineering, y_Domineering, b_Domineering)), false)\n"
  "+ true ->\n"
  "delta;\n"
  "\n"
  "init P((Empty |> Empty |> Empty |> Empty |> Empty |> []) |> (Empty |> Empty |> Empty |> Empty |> Empty |> []) |> (Empty |> Empty |> Empty |> Empty |> Empty |> []) |> (Empty |> Empty |> Empty |> Empty |> Empty |> []) |> (Empty |> Empty |> Empty |> Empty |> Empty |> []) |> [], true);\n"
  ;
const std::string removed_9 = "";

inline
bool is_linear(const process::process_specification& pspec)
{
  return process::detail::linear_process_expression_visitor().is_linear(pspec);
}

void test_constelm(const std::string& spec_text, const std::string& expected_result)
{
  specification s0;
  process::process_specification pspec = process::parse_process_specification(spec_text);
  if (is_linear(pspec))
  {
    s0 = parse_linear_process_specification(spec_text);
  }
  else
  {
    s0 = mcrl22lps(spec_text);
  }
  data::rewriter datar(s0.data());
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
  test_constelm(case_9, removed_9);
  core::garbage_collect();

  return 0;
}
