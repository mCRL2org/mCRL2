// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file list_test.cpp
/// \brief Basic regression test for quantifier expressions.

#include <boost/test/minimal.hpp>

#include "mcrl2/data/detail/rewrite_strategies.h"
#include "mcrl2/data/fset.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/standard.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::sort_list;

void quantifier_expression_test(const std::string& expr1_text, const std::string& expr2_text, const data_specification& dataspec, const rewriter& r)
{
  data_expression expr1 = parse_data_expression(expr1_text, dataspec);
  data_expression expr2 = parse_data_expression(expr2_text, dataspec);
  std::cout << "Testing " << expr1_text << " <-> " << expr2_text << std::endl;
  if (r(expr1) != r(expr2))
  {
    std::cout << "--- ERROR ---\n";
    std::cout << " expr1    = " << expr1 << std::endl;
    std::cout << " expr2    = " << expr2 << std::endl;
    std::cout << " r(expr1) = " << r(expr1) << std::endl;
    std::cout << " r(expr2) = " << r(expr2) << std::endl;
    BOOST_CHECK(r(expr1) == r(expr2));
  }
}

void quantifier_expression_test(mcrl2::data::rewrite_strategy s)
{
  data_specification dataspec;
  rewriter r(dataspec, s);

  // tests for Bool
  quantifier_expression_test("exists x: Bool. x == false", "true", dataspec, r);
  quantifier_expression_test("forall x: Bool. x == false", "false", dataspec, r);
  quantifier_expression_test("exists x: Bool. x == true", "true", dataspec, r);
  quantifier_expression_test("forall x: Bool. x == true", "false", dataspec, r);
  quantifier_expression_test("forall x: Bool. x == true && x==false", "false", dataspec, r);
  quantifier_expression_test("exists x: Bool. x == true && x==false", "false", dataspec, r);
  quantifier_expression_test("forall x: Bool. x == true || x==false", "true", dataspec, r);
  quantifier_expression_test("exists x: Bool. x == true || x==false", "true", dataspec, r);
  quantifier_expression_test("forall x:Bool.exists y: Bool. x == y", "true", dataspec, r);
  quantifier_expression_test("exists x: Bool.forall y:Bool.x == y", "false", dataspec, r);
  quantifier_expression_test("forall b: Bool. b", "false", dataspec, r);

  // tests for Pos / Nat
  dataspec.add_context_sort(sort_nat::nat());
  dataspec.add_context_sort(sort_set::set_(sort_nat::nat()));
  r = rewriter(dataspec, s);
  quantifier_expression_test("exists x: Nat. (  x in {1,2,25,600} && 25 == x )", "true", dataspec, r);

  // This test depends on the naming of variables in the enumerator
  // quantifier_expression_test("forall x: Nat. exists y: Nat. y == x", "true", dataspec, r);

  quantifier_expression_test("forall x: Nat. x == 3", "false", dataspec, r);
  quantifier_expression_test("exists x: Nat. x == 3", "true", dataspec, r);
  quantifier_expression_test("forall x: Pos. exists y: Pos.x == y+1", "false", dataspec, r);
  /* Test 15. Test whether elimination of quantifiers also happens inside a term. */
  quantifier_expression_test("(exists x_0: Bool. false) && (forall x_0: Nat. true)", "false", dataspec, r);
  quantifier_expression_test("(forall x_0: Pos. true) || (exists x_0: Bool. false)", "true", dataspec, r);
  /* The test below is too complex for the enumerator to solve.
  quantifier_expression_test("forall x: Pos. exists y: Nat.x == y+1", "true", dataspec, r);
  */

/* These tests do not work with the new enumerator, since it does not handle data equalities.
  quantifier_expression_test("forall x:Pos.exists y1,y2:Pos.x==y1+y2", "false", dataspec, r);
  quantifier_expression_test("forall x:Nat.exists y1,y2:Nat.x==y1+y2", "true", dataspec, r);
*/

  // tests for struct / List
  dataspec = parse_data_specification("sort S = struct s1?is_s1 | s2?is_s2;"
                                      "sort T = struct t;"
                                      "sort L = List(T);");
  r = rewriter(dataspec, s);

  quantifier_expression_test("exists s: S.( is_s1(s) && is_s2(s) )", "false", dataspec, r);
  quantifier_expression_test("exists s: S.( s == s2 && is_s2(s) )", "true", dataspec, r);
  quantifier_expression_test("forall y: T. y in [t]", "true", dataspec, r);

/* Should work but takes too much time.
  dataspec = parse_data_specification(
  " sort A = struct ac( first: Bool, args: List(Bool));"
  " "
  " map COMM: List(Bool)#Bool#List(A) -> List(A);"
  "     COMM: List(Bool)#Bool#List(A)#(List(Bool)->FBag(Bool))#FBag(Bool) -> List(A);"
  "     MAC: List(Bool) -> FBag(Bool);"
  "     PART: List(A)#(List(Bool) -> FBag(Bool)) -> (List(Bool) -> FBag(Bool));"
  "     ELM: List(Bool)#Bool#List(A)#List(Bool)#List(Bool) -> List(A);"
  "     RM: A#List(A)->List(A);"
  " var func: List(Bool)->FBag(Bool);"
  "     as: List(A);"
  "     ca: Bool;"
  "     cal: List(Bool);"
  "     cal_const: List(Bool);"
  "     al: Bool;"
  "     lsa: List(A);"
  "     m: FBag(Bool);"
  "     args: List(Bool);"
  "     a, b: A;"
  " eqn PART( [] , func ) = func;"
  "     PART( a |> as , func ) = PART( as, func[ args(a) -> func(args(a)) + {first(a):1}]  );"
  "     MAC( [] ) = {:};"
  "     MAC( ca |> cal ) = {ca:1} + MAC(cal);"
  " "
  "     COMM( cal, al, lsa ) = COMM( cal, al, lsa,  PART( lsa, lambda x: List(Bool). {:} ), MAC(cal) );"
  "     COMM( cal, al, [] , func, m ) = [];"
  "     COMM( cal, al, a |> lsa, func, m ) = if( m <= func(args(a)), "
  "                                                      ELM( cal, al, a |> lsa, args(a) , cal ) , "
  "                                                      a |> COMM( cal, al, lsa, func, m)"
  "                                                    );"
  "     ELM( [] , al, lsa, args, cal_const ) = [ac(al, args)] ++ COMM( cal_const, al, lsa );"
  "     ELM( ca |> cal, al, lsa, args, cal_const ) = ELM( cal, al , RM( ac( ca ,args), lsa), args, cal_const );"
  "     RM( a, [] ) = [];"
  "     RM( a, b |> lsa ) = if(a == b , lsa,  b |> RM( a, lsa)) ;"
  );
  r = rewriter(dataspec, s);

  quantifier_expression_test("exists x_0: List(A). x_0 == [ac( false, []), ac(true, []), ac(false, [])] && [ac(true, []), ac(true, [])] == COMM([false, false], true, x_0, PART(x_0, lambda x: List(Bool). {:}), {false: 2}) ", "true", dataspec, r);
  quantifier_expression_test("exists x_0: List(A). [ac(true, []), ac(true, [])] == "
       "        COMM([false, false], true, x_0, PART(x_0, lambda x: List(Bool). {:}), {false: 2}) &&  "
       "        x_0 == [ac( false, []), ac(true, []), ac(false, [])]", "true", dataspec, r);
*/

  // tests for Set
  dataspec = parse_data_specification( "sort A = Set(Bool);");
  r = rewriter(dataspec, s);

  /* Test that exists and forall over a non enumerable sort (situation winter 2012)
     with a trivial predicate can still be reduced, by removing the variable. */
  quantifier_expression_test("exists x:Set(Bool). x==x", "true", dataspec, r);
  quantifier_expression_test("forall x:Set(Bool). x==x", "true", dataspec, r);
  quantifier_expression_test("exists x:Set(Bool). x!=x", "false", dataspec, r);
  quantifier_expression_test("forall x:Set(Bool). x!=x", "false", dataspec, r);
}

void quantifier_in_rewrite_rules_test(mcrl2::data::rewrite_strategy s)
{
  // The test below checks whether bound variables in rewrite rules are properly renamed
  // when substituting variables. In concreto, if the y in the eqn for f is substituted in
  // the body of g(x), then if y is substituted for x, the rhs of g(x) reduces to y!=y, or false.
  // The correct answer however is true, as f states taht for every boolean y there is an y' that
  // is not equal to it.
  data_specification dataspec = parse_data_specification(
                    "map f:Bool;\n"
                    "    g:Bool->Bool;\n"
                    "var x:Bool;\n"
                    "eqn f=forall y:Bool.g(y);\n"
                    "    g(x)=exists y:Bool.x!=y;\n");

  rewriter r(dataspec, s);
  quantifier_expression_test("f", "true", dataspec, r);
}


int test_main(int argc, char** argv)
{
  auto strategies = data::detail::get_test_rewrite_strategies(false);
  for (const auto& strategy: strategies)
  {
    std::clog << "  Strategy: " << strategy << std::endl;
    quantifier_expression_test(strategy);
    quantifier_in_rewrite_rules_test(strategy);
  }

  return EXIT_SUCCESS;
}
