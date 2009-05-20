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

#define MCRL2_LPSCONSTELM_DEBUG

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "mcrl2/core/text_utility.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/constelm.h"
#include "mcrl2/lps/specification.h"
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

std::string case_6a =
  "%--- case 6 linearized ---%                                        \n"
  "act  action: Nat;                                                  \n"
  "                                                                   \n"
  "proc P(s3_P: Pos, i_P: Nat) =                                      \n"
  "       sum e_P: Bool.                                              \n"
  "         (if(e_P, true, true) && if(e_P, s3_P == 2, s3_P == 1)) -> \n"
  "         action(i_P) .                                             \n"
  "         P(if(e_P, 1, 2), i_P)                                     \n"
  "     + true ->                                                     \n"
  "         delta;                                                    \n"
  "                                                                   \n"
  "init P(1, 0);                                                      \n"
  ;
const std::string removed_6a = "i_P";

std::string case_6b =
  "%--- case 6 linearized no-cluster ---%                             \n"
  "act  action: Nat;                                                  \n"
  "                                                                   \n"
  "proc P(s3_P: Pos, i_P: Nat) =                                      \n"
  "       (s3_P == 2) ->                                              \n"
  "         action(i_P) .                                             \n"
  "         P(1, i_P)                                                 \n"
  "     + (s3_P == 1) ->                                              \n"
  "         action(i_P) .                                             \n"
  "         P(2, i_P)                                                 \n"
  "     + true ->                                                     \n"
  "         delta;                                                    \n"
  "                                                                   \n"
  "init P(1, 0);                                                      \n"
  ;
const std::string removed_6b = "i_P";

// % Test Case 7 -- Free Variables
// %
// % Process parameters s3, j and i are marked constant and all occurrences are
// % substituted.
// %
// % If \"--no-condition\" is used only j is marked constant and all occurrences of
// % are substituted.
//
// act action :Nat;
//
// proc P(i,j: Nat) = (i > 5) -> action(i). P(i+1,j) +
//                    (i == 5) -> action(j). Q(j);
//      Q(i: Nat)   = action(i). Q(i);
//
// init P(0,0);
//
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %% NOTE:                                                  %%
// %% =====                                                  %%
// %%                                                        %%
// %% Use: mcrl22lps --no-cluster $DIR$/case7.mcrl2          %%
// %%                                                        %%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%                                                        %%
// %% var  freevar,freevar0: Nat;                            %%
// %% proc P(s3: Pos, j,i: Nat) =                            %%
// %%        (s3 == 2) ->                                    %%
// %%          action(i) .                                   %%
// %%          P(s3 := 2, j := freevar0)                     %%
// %%      + (s3 == 1 && 5 < i ) ->                          %%
// %%          action(i) .                                   %%
// %%          P(s3 := 1, i := i + 1)                        %%
// %%      + (s3 == 1 && i == 5) ->                          %%
// %%          action(j) .                                   %%
// %%          P(s3 := 2, j := freevar, i := j);             %%
// %%                                                        %%
// %% init P(s3 := 1, j := 0, i := 0);                       %%
// %%                                                        %%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

std::string case_7 =
  "act  action: Nat;                 \n"
  "                                  \n"
  "var  dc,dc1: Nat;                 \n"
  "proc P(s3_P: Pos, j_P,i_P: Nat) = \n"
  "       (s3_P == 2) ->             \n"
  "         action(i_P) .            \n"
  "         P(2, dc1, i_P)           \n"
  "     + (s3_P == 1 && i_P == 5) -> \n"
  "         action(j_P) .            \n"
  "         P(2, dc, j_P)            \n"
  "     + (s3_P == 1 && i_P > 5) ->  \n"
  "         action(i_P) .            \n"
  "         P(1, j_P, i_P + 1);      \n"
  "                                  \n"
  "init P(1, 0, 0);                  \n"
  ;
const std::string removed_7 = "i_P j_P s3_P";

// % Test Case 8 -- Free Variables                                                 
// %                                                                               
// % No constant parameters are found                                              
// %                                                                               
// % lpsconstelm cannot detect (i==5)                                              
//                                                                                 
// act action: Nat;                                                                
//                                                                                 
// proc X(i: Nat)   = (i <  5) -> action(i).X(i+1) +                               
//                    (i == 5) -> action(i).Y(i, i);                               
//      Y(i,j: Nat) = action(j).Y(i,j);                                            
//                                                                                 
// init X(0);                                                                      
//                                                                                 
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %% NOTE:                                                  %%
// %% =====                                                  %%
// %%                                                        %%
// %% Use: mcrl22lps --no-cluster $DIR$/case8.mcrl2          %%
// %%                                                        %%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%                                                        %%
// %% act  action: Nat;                                      %%
// %%                                                        %%
// %% var  freevar0: Nat;                                    %%
// %% proc P(s3: Pos, i,j: Nat) =                            %%
// %%       (s3 == 2) ->                                     %%
// %%         action(j) .                                    %%
// %%         P(s3 := 2)                                     %%
// %%     + (s3 == 1 && i < 5) ->                            %%
// %%         action(i) .                                    %%
// %%         P(s3 := 1, i := i + 1, j := freevar0)          %%
// %%     + (s3 == 1 && i == 5) ->                           %%
// %%         action(i) .                                    %%
// %%         P(s3 := 2, j := i);                            %%
// %%                                                        %%
// %% var  freevar: Nat;                                     %%
// %% init P(s3 := 1, i := 0, j := freevar);                 %%
// %%                                                        %%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
std::string case_8 =
  "act  action: Nat;                 \n"
  "                                  \n"
  "var  dc1: Nat;                    \n"
  "proc P(s3_X: Pos, i_X,j_X: Nat) = \n"
  "       (s3_X == 2) ->             \n"
  "         action(j_X) .            \n"
  "         P(2, i_X, j_X)           \n"
  "     + (s3_X == 1 && i_X == 5) -> \n"
  "         action(i_X) .            \n"
  "         P(2, i_X, i_X)           \n"
  "     + (s3_X == 1 && i_X < 5) ->  \n"
  "         action(i_X) .            \n"
  "         P(1, i_X + 1, dc1);      \n"
  "                                  \n"
  "var  dc: Nat;                     \n"
  "init P(1, 0, dc);                 \n"
  ;
const std::string removed_8 = "";

// examples/games/domineering.mcrl2
std::string case_9 =
  "sort Position = struct F | E;\n"
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
  "At(x, y, [])  =  E;\n"
  "y == 1  ->  At(x, y, r |> b)  =  At(x, r);\n"
  "y > 1  ->  At(x, y, r |> b)  =  At(x, Int2Pos(y - 1), b);\n"
  "At(y, [])  =  E;\n"
  "x == 1  ->  At(x, p |> r)  =  p;\n"
  "x > 1  ->  At(x, p |> r)  =  At(Int2Pos(x - 1), r);\n"
  "y == 1  ->  Put(p, x, y, r |> b)  =  Put(p, x, r) |> b;\n"
  "y > 1  ->  Put(p, x, y, r |> b)  =  r |> Put(p, x, Int2Pos(y - 1), b);\n"
  "x == 1  ->  Put(p, x, p' |> r)  =  p |> r;\n"
  "x > 1  ->  Put(p, x, p' |> r)  =  p' |> Put(p, Int2Pos(x - 1), r);\n"
  "y == M  ->  NoVerticalSpot(x, y,b)  =  true;\n"
  "x < N && y < M  ->  NoVerticalSpot(x, y, b)  =  (At(x, y, b) == F || At(x, y + 1, b) == F) && NoVerticalSpot(x + 1, y, b);\n"
  "x == N && y < M  ->  NoVerticalSpot(x, y, b)  =  (At(x, y, b) == F || At(x, y + 1, b) == F) && NoVerticalSpot(1, y + 1, b);\n"
  "x == N  ->  NoHorizontalSpot(x, y,b)  =  true;\n"
  "x < N && y < M  ->  NoHorizontalSpot(x, y, b)  =  (At(x, y, b) == F || At(x + 1, y, b) == F) && NoHorizontalSpot(x, y + 1, b);\n"
  "x < N && y == M  ->  NoHorizontalSpot(x, y, b)  =  (At(x, y, b) == F || At(x + 1, y, b) == F) && NoHorizontalSpot(x + 1, 1, b);\n"
  "\n"
  "act  Player1,Player2: Pos # Pos # Pos # Pos;\n"
  "Player1Wins,Player2Wins;\n"
  "\n"
  "proc P(b: List(List(struct F | E)), c: Bool) =\n"
  "(c && (At(1,1,b) == F || At(1,2,b) == F) && (At(2,1,b) == F || At(2,2,b) == F) && (At(3,1,b) == F || At(3,2,b) == F) && (At(4,1,b) == F || At(4,2,b) == F) && (At(5,1,b) == F || At(5,2,b) == F) && (At(1,2,b) == F || At(1,3,b) == F) && (At(2,2,b) == F || At(2,3,b) == F) && (At(3,2,b) == F || At(3,3,b) == F) && (At(4,2,b) == F || At(4,3,b) == F) && (At(5,2,b) == F || At(5,3,b) == F) && (At(1,3,b) == F || At(1,4,b) == F) && (At(2,3,b) == F || At(2,4,b) == F) && (At(3,3,b) == F || At(3,4,b) == F) && (At(4,3,b) == F || At(4,4,b) == F) && (At(5,3,b) == F || At(5,4,b) == F) && (At(1,4,b) == F || At(1,5,b) == F) && (At(2,4,b) == F || At(2,5,b) == F) && (At(3,4,b) == F || At(3,5,b) == F) && (At(4,4,b) == F || At(4,5,b) == F) && At(5,4,b) == F || At(5,5,b) == F) ->\n"
  "Player2Wins .\n"
  "P(b, c)\n"
  "+ sum x1,y1: Pos.\n"
  "(!c && x1 < 5 && y1 <= 5 && At(x1, y1, b) == E && At(succ(x1), y1, b) == E) ->\n"
  "Player2(x1, y1, x1 + 1, y1) .\n"
  "P(Put(F, succ(x1), y1, Put(F, x1, y1, b)), true)\n"
  "+ (!c && (At(1,1,b) == F || At(2,1,b) == F) && (At(1,2,b) == F || At(2,2,b) == F) && (At(1,3,b) == F || At(2,3,b) == F) && (At(1,4,b) == F || At(2,4,b) == F) && (At(1,5,b) == F || At(2,5,b) == F) && (At(2,1,b) == F || At(3,1,b) == F) && (At(2,2,b) == F || At(3,2,b) == F) && (At(2,3,b) == F || At(3,3,b) == F) && (At(2,4,b) == F || At(3,4,b) == F) && (At(2,5,b) == F || At(3,5,b) == F) && (At(3,1,b) == F || At(4,1,b) == F) && (At(3,2,b) == F || At(4,2,b) == F) && (At(3,3,b) == F || At(4,3,b) == F) && (At(3,4,b) == F || At(4,4,b) == F) && (At(3,5,b) == F || At(4,5,b) == F) && (At(4,1,b) == F || At(5,1,b) == F) && (At(4,2,b) == F || At(5,2,b) == F) && (At(4,3,b) == F || At(5,3,b) == F) && (At(4,4,b) == F || At(5,4,b) == F) && At(4,5,b) == F || At(5,5,b) == F) ->\n"
  "Player1Wins .\n"
  "P(b, c)\n"
  "+ sum x,y: Pos.\n"
  "(c && x <= 5 && y < 5 && At(x, y, b) == E && At(x, succ(y), b) == E) ->\n"
  "Player1(x, y, x, y + 1) .\n"
  "P(Put(F, x, succ(y), Put(F, x, y, b)), false)\n"
  "+ true ->\n"
  "delta;\n"
  "\n"
  "init P((E |> E |> E |> E |> E |> []) |> (E |> E |> E |> E |> E |> []) |> (E |> E |> E |> E |> E |> []) |> (E |> E |> E |> E |> E |> []) |> (E |> E |> E |> E |> E |> []) |> [], true);\n"
  ;
const std::string removed_9 = "";

void test_constelm(const std::string& spec_text, const std::string& expected_result)
{
  specification s0 = parse_linear_process_specification(spec_text);
  data::rewriter datar(s0.data());
  specification s1 = constelm(s0, datar, true);
  variable_list v0 = s0.process().process_parameters();
  variable_list v1 = s1.process().process_parameters();

  // create a set of strings set1 that contains the names of expected removed parameters and free
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
    std::clog << "--- failed test ---\n";
    std::clog << spec_text << std::endl;
    std::clog << "expected result " << boost::algorithm::join(set1, " ") << std::endl;
    std::clog << "computed result " << boost::algorithm::join(set2, " ") << std::endl;
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
  test_constelm(case_6a, removed_6a);
  core::garbage_collect();
  test_constelm(case_6b, removed_6b);
  core::garbage_collect();
// TODO: uncomment these tests when free variables can be parsed
//  test_constelm(case_7, removed_7);
  core::garbage_collect();
//  test_constelm(case_8, removed_8);
  core::garbage_collect();
  test_constelm(case_9, removed_9);
  core::garbage_collect();

  return 0;
}
