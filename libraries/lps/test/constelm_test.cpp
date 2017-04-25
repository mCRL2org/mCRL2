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

//#define MCRL2_LPSCONSTELM_DEBUG

#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/constelm.h"
#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/utilities/text_utility.h"
#include <boost/algorithm/string.hpp>
#include <boost/test/minimal.hpp>
#include <iostream>
#include <string>

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
const std::string expected_1 = "process_parameter_names =";

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
const std::string expected_2 = "process_parameter_names = i";

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
const std::string expected_3 = "process_parameter_names = i";

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
const std::string expected_4 = "process_parameter_names = i";

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
const std::string expected_5 = "process_parameter_names = i";

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
const std::string expected_6a = "process_parameter_names = s3_P";

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
const std::string expected_6b = "process_parameter_names = s3_P";

// % Test Case 7 -- Free Variables
// %

std::string case_7 =
  "glob  v: Nat ;                     \n"
  "                                   \n"
  "act a: Nat ;                       \n"
  "                                   \n"
  "proc P(i, j: Nat) =                \n"
  "       (i == j) -> a(i) . P(1, 1)  \n"
  "       ;                           \n"
  "                                   \n"
  "init P(i = 1, j = v) ;             \n"
  ;
const std::string expected_7 = "process_parameter_names = j";

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
const std::string expected_8 = "process_parameter_names = ";

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
const std::string expected_9 = "process_parameter_names = b, c";

// Test case provided by Jan Friso Groote (2-9-2009). It is based on the process
//
// act  a:Bool;
// proc X(p:List(Bool))=sum b:Bool.a(head(p)).X(b|>tail(p)); init X([true]);
//
// The list has always length one. After application of lpsconstelm, only one of the
// three parameters should remain.
//
// Before linearization:
//
// sort S11;
//
// cons c_1,c_2: S11;
//
// map  C_S11_1: S11 # List(Bool) # List(Bool) -> List(Bool);
//      pi_S11_2: List(Bool) -> List(Bool);
//      pi_S11_1: List(Bool) -> Bool;
//      Det_S11_1: List(Bool) -> S11;
//
// var  y3,y4,d3: List(Bool);
//      y2: S11;
//      d2: Bool;
// eqn  C_S11_1(c_1, y3, y4)  =  y3;
//      C_S11_1(c_2, y3, y4)  =  y4;
//      C_S11_1(y2, y4, y4)  =  y4;
//      Det_S11_1([])  =  c_1;
//      Det_S11_1(d2 |> d3)  =  c_2;
//      pi_S11_1(d2 |> d3)  =  d2;
//      pi_S11_2(d2 |> d3)  =  d3;
//
// act  a: Bool;
//
// proc P(S1_pp1: S11, S1_pp2: Bool, S1_pp3: List(Bool)) =
//        sum b_X: Bool.
//          a(head(C_S11_1(S1_pp1, [], S1_pp2 |> S1_pp3))) .
//          P(S1_pp1 = Det_S11_1(b_X |> tail(C_S11_1(S1_pp1, [], S1_pp2 |> S1_pp3))), S1_pp2 = pi_S11_1(b_X |> tail(C_S11_1(S1_pp1, [], S1_pp2 |> S1_pp3))), S1_pp3 = pi_S11_2(b_X |> tail(C_S11_1(S1_pp1, [], S1_pp2 |> S1_pp3))))
//      + true ->
//          delta;
//
// init P(Det_S11_1(true |> []), pi_S11_1(true |> []), pi_S11_2(true |> []));
//
std::string case_10 =
  "sort S11;                                                                                                    \n"
  "                                                                                                             \n"
  "cons c_1,c_2: S11;                                                                                           \n"
  "                                                                                                             \n"
  "map  C_S11_1: S11 # List(Bool) # List(Bool) -> List(Bool);                                                   \n"
  "     pi_S11_2: List(Bool) -> List(Bool);                                                                     \n"
  "     pi_S11_1: List(Bool) -> Bool;                                                                           \n"
  "     Det_S11_1: List(Bool) -> S11;                                                                           \n"
  "                                                                                                             \n"
  "var  y3,y4,d3: List(Bool);                                                                                   \n"
  "     y2: S11;                                                                                                \n"
  "     d2: Bool;                                                                                               \n"
  "eqn  C_S11_1(c_1, y3, y4)  =  y3;                                                                            \n"
  "     C_S11_1(c_2, y3, y4)  =  y4;                                                                            \n"
  "     C_S11_1(y2, y4, y4)  =  y4;                                                                             \n"
  "     Det_S11_1([])  =  c_1;                                                                                  \n"
  "     Det_S11_1(d2 |> d3)  =  c_2;                                                                            \n"
  "     pi_S11_1(d2 |> d3)  =  d2;                                                                              \n"
  "     pi_S11_2(d2 |> d3)  =  d3;                                                                              \n"
  "                                                                                                             \n"
  "act  a: Bool;                                                                                                \n"
  "                                                                                                             \n"
  "proc P(S1_pp1_P: S11, S1_pp2_P: Bool, S1_pp3_P: List(Bool)) =                                                \n"
  "       sum b_X_P: Bool.                                                                                      \n"
  "         true ->                                                                                             \n"
  "         a(head(C_S11_1(S1_pp1_P, [], S1_pp2_P |> S1_pp3_P))) .                                              \n"
  "         P(S1_pp1_P = c_2, S1_pp2_P = b_X_P, S1_pp3_P = tail(C_S11_1(S1_pp1_P, [], S1_pp2_P |> S1_pp3_P)));  \n"
  "                                                                                                             \n"
  "init P(Det_S11_1(true |> []), pi_S11_1(true |> []), pi_S11_2(true |> []));                                   \n"
  ;

const std::string expected_10 = "process_parameter_count = 1";

void test_constelm(const std::string& message, const std::string& spec_text, const std::string& expected_result)
{
  specification spec = parse_linear_process_specification(spec_text);
  data::rewriter R(spec.data());
  bool instantiate_free_variables = false;
  constelm(spec, R, instantiate_free_variables);
  lps::detail::specification_property_map<> info(spec);
  BOOST_CHECK(data::detail::compare_property_maps(message, info, expected_result));
}

void test_constelm()
{
  test_constelm("case_1" , case_1,  expected_1);
  test_constelm("case_2" , case_2,  expected_2);
  test_constelm("case_3" , case_3,  expected_3);
  test_constelm("case_4" , case_4,  expected_4);
  test_constelm("case_5" , case_5,  expected_5);
  test_constelm("case_6a", case_6a, expected_6a);
  test_constelm("case_6b", case_6b, expected_6b);
  test_constelm("case_7" , case_7,  expected_7);
  // test_constelm("case_8" , case_8,  expected_8);
  test_constelm("case_9" , case_9,  expected_9);
  test_constelm("case_10" , case_10,  expected_10);
}

void test_abp()
{
  stochastic_specification spec = linearise(lps::detail::ABP_SPECIFICATION());
  data::rewriter R(spec.data());
  bool instantiate_free_variables = false;
  constelm(spec, R, instantiate_free_variables);
  BOOST_CHECK(check_well_typedness(spec));
}

void test_stochastic_specification()
{
  std::string text =
    "act action :Nat;                                     \n"
    "                                                     \n"
    "proc P(i, j: Nat) = true  -> action(j). P(i+1,j)   + \n"
    "                    false -> action(j). P(i+1,j+1);  \n"
    "                                                     \n"
    "init P(0,5);                                         \n"
    "                                                     \n"
    ;
  std::string expected_result = "process_parameter_names = i";
  stochastic_specification spec;
  parse_lps(text, spec);
  data::rewriter R(spec.data());
  bool instantiate_free_variables = false;
  constelm(spec, R, instantiate_free_variables);
  lps::detail::specification_property_map<stochastic_specification> info(spec);
  BOOST_CHECK(data::detail::compare_property_maps("test_stochastic_specification", info, expected_result));
}

int test_main(int argc, char* argv[])
{
  test_constelm();
  test_abp();
  test_stochastic_specification();

  return 0;
}

