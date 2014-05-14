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

#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/list.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/fset.h"
#include "mcrl2/data/standard.h"

#include "mcrl2/utilities/test_utilities.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::sort_list;

typedef std::vector<rewrite_strategy > rewrite_strategy_vector;

static
void quantifier_expression_test(mcrl2::data::rewrite_strategy s)
{
  data_specification specification;

  specification = parse_data_specification(
                                       "sort S = struct s1?is_s1 | s2?is_s2;"
                                       "sort T = struct t;"
                                       "sort L = List(T);");

  specification.add_context_sort(sort_nat::nat());
  specification.add_context_sort(sort_set::set_(sort_nat::nat()));

  rewriter r(specification, s);

  /* Test 1*/
  data_expression t1d1 = parse_data_expression("exists x: Bool. x == false");
  data_expression t1d2 = parse_data_expression("true");
  BOOST_CHECK(r(t1d1) == r(t1d2));

  /* Test 1a*/
  data_expression t1ad1 = parse_data_expression("forall x: Bool. x == false");
  data_expression t1ad2 = parse_data_expression("false");
  BOOST_CHECK(r(t1ad1) == r(t1ad2));

  /* Test 1b*/
  data_expression t1bd1 = parse_data_expression("exists x: Bool. x == true");
  data_expression t1bd2 = parse_data_expression("true");
  BOOST_CHECK(r(t1bd1) == r(t1bd2));

  /* Test 1c*/
  data_expression t1cd1 = parse_data_expression("forall x: Bool. x == true");
  data_expression t1cd2 = parse_data_expression("false");
  BOOST_CHECK(r(t1cd1) == r(t1cd2));

  /* Test 1d*/
  data_expression t1dd1 = parse_data_expression("forall x: Bool. x == true && x==false");
  data_expression t1dd2 = parse_data_expression("false");
  BOOST_CHECK(r(t1dd1) == r(t1dd2));

  /* Test 1e*/
  data_expression t1ed1 = parse_data_expression("exists x: Bool. x == true && x==false");
  data_expression t1ed2 = parse_data_expression("false");
  BOOST_CHECK(r(t1ed1) == r(t1ed2));

  /* Test 1f*/
  data_expression t1fd1 = parse_data_expression("forall x: Bool. x == true || x==false");
  data_expression t1fd2 = parse_data_expression("true");
  BOOST_CHECK(r(t1fd1) == r(t1fd2));

  /* Test 1g*/
  data_expression t1gd1 = parse_data_expression("exists x: Bool. x == true || x==false");
  data_expression t1gd2 = parse_data_expression("true");
  BOOST_CHECK(r(t1gd1) == r(t1gd2));

  /* Test 2*/
  data_expression t2d1 = parse_data_expression("exists x: Nat. (  x in {1,2,25,600} && 25 == x )", specification);
  data_expression t2d2 = parse_data_expression("true");
  BOOST_CHECK(r(t2d1) == r(t2d2));

  /* Test 3*/
  data_expression t3d1 = parse_data_expression("forall x: Nat. exists y: Nat. y == x");
  data_expression t3d2 = parse_data_expression("true");
  BOOST_CHECK(r(t3d1) == r(t3d2));

  /* Test 4*/
  data_expression t4d1 = parse_data_expression("exists s: S.( is_s1(s) && is_s2(s) )", specification);
  data_expression t4d2 = parse_data_expression("false");
  BOOST_CHECK(r(t4d1) == r(t4d2));


  /* Test 5*/
  data_expression t5d1 = parse_data_expression("exists s: S.( s == s2 && is_s2(s) )", specification);
  data_expression t5d2 = parse_data_expression("true");
  BOOST_CHECK(r(t5d1) == r(t5d2));

  /* Test 6*/
  data_expression t6d1 = parse_data_expression("forall y: T. y in [t]", specification);
  data_expression t6d2 = parse_data_expression("true");
  BOOST_CHECK(r(t6d1) == r(t6d2));

  /* Test 7*/
  data_expression t7d1 = parse_data_expression("forall b: Bool. b", specification);
  data_expression t7d2 = parse_data_expression("false");
  BOOST_CHECK(r(t7d1) == r(t7d2));

  /* Test 8*/
  data_expression t8d1 = parse_data_expression("forall x: Nat. x == 3", specification);
  data_expression t8d2 = parse_data_expression("false");
  BOOST_CHECK(r(t8d1) == r(t8d2));

  /* Test 9*/
  data_expression t9d1 = parse_data_expression("exists x: Nat. x == 3", specification);
  data_expression t9d2 = parse_data_expression("true");
  BOOST_CHECK(r(t9d1) == r(t9d2));

  /* Test 10*/
  /* The test below is too complex for the enumerator to solve.
  data_expression t10d1 = parse_data_expression("forall x: Pos. exists y: Nat.x == y+1", specification);
  data_expression t10d2 = parse_data_expression("true");
  BOOST_CHECK(r(t10d1) == r(t10d2));
  */

  /* Test 11*/
  data_expression t11d1 = parse_data_expression("forall x: Pos. exists y: Pos.x == y+1", specification);
  data_expression t11d2 = parse_data_expression("false");
  BOOST_CHECK(r(t11d1) == r(t11d2));

  /* Test 12*/
  data_expression t12d1 = parse_data_expression("forall x: Pos. exists y: Pos.x == y+1", specification);
  data_expression t12d2 = parse_data_expression("false");
  BOOST_CHECK(r(t12d1) == r(t12d2));

  /* Test 13*/
  data_expression t13d1 = parse_data_expression("forall x:Pos.exists y1,y2:Pos.x==y1+y2", specification);
  data_expression t13d2 = parse_data_expression("false");
  BOOST_CHECK(r(t13d1) == r(t13d2));

  /* Test 14*/
  data_expression t14d1 = parse_data_expression("forall x:Nat.exists y1,y2:Nat.x==y1+y2", specification);
  data_expression t14d2 = parse_data_expression("true");
  BOOST_CHECK(r(t14d1) == r(t14d2));

  /* Test 15. Test whether elimination of quantifiers also happens inside a term. */
  data_expression t15d1 = parse_data_expression("(exists x_0: Bool. false) && (forall x_0: Nat. true)", specification);
  data_expression t15d2 = parse_data_expression("false");
  BOOST_CHECK(r(t15d1) == r(t15d2));

  /* Test 16. Similar test as test 15 */
  data_expression t16d1 = parse_data_expression("(forall x_0: Pos. true) || (exists x_0: Bool. false)", specification);
  data_expression t16d2 = parse_data_expression("true");
  BOOST_CHECK(r(t16d1) == r(t16d2));

  data_specification spec_1;
  spec_1 = parse_data_specification(
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


  /* Test 17. */
  r = rewriter(spec_1, s);
  data_expression t17d1 = parse_data_expression("exists x_0: List(A). x_0 == [ac( false, []), ac(true, []), ac(false, [])] && [ac(true, []), ac(true, [])] == COMM([false, false], true, x_0, PART(x_0, lambda x: List(Bool). {:}), {false: 2}) ", spec_1);
  data_expression t17d2 = parse_data_expression("true");
  // BOOST_CHECK(r(t17d1) == r(t17d2)); Should work but takes too much time.

  /* Test 18. Similar test as test 17.
     The difference is in the structure of the body of the exists: Arguments left and right of the conjunct are swapped.
  */
  data_expression t18d1 = parse_data_expression(
       "exists x_0: List(A). [ac(true, []), ac(true, [])] == "
       "        COMM([false, false], true, x_0, PART(x_0, lambda x: List(Bool). {:}), {false: 2}) &&  "
       "        x_0 == [ac( false, []), ac(true, []), ac(false, [])]", spec_1);
  data_expression t18d2 = parse_data_expression("true");
  // BOOST_CHECK(r(t18d1) == r(t18d2)); Should work but takes too much time.

  /* Test 19. Test that exists and forall over a non enumerable sort (situation winter 2012)
              with a trivial predicate can still be reduced, by removing the variable. */

  spec_1 = parse_data_specification( "sort A = Set(Bool);");
  r = rewriter(spec_1, s);
  data_expression t19true = parse_data_expression("true");
  data_expression t19a = parse_data_expression(
       "exists x:Set(Bool). x==x", spec_1);
  BOOST_CHECK(r(t19a) == r(t19true));
  data_expression t19b = parse_data_expression(
       "forall x:Set(Bool). x==x", spec_1);
  BOOST_CHECK(r(t19b) == r(t19true));
  data_expression t19false = parse_data_expression("false");
  data_expression t19c = parse_data_expression(
       "exists x:Set(Bool). x!=x", spec_1);
  BOOST_CHECK(r(t19c) == r(t19false));
  data_expression t19d = parse_data_expression(
       "forall x:Set(Bool). x!=x", spec_1);
  BOOST_CHECK(r(t19d) == r(t19false));
}

static void quantifier_in_rewrite_rules_test(mcrl2::data::rewrite_strategy s)
{
  // The test below checks whether bound variables in rewrite rules are properly renamed
  // when substituting variables. In concreto, if the y in the eqn for f is substituted in
  // the body of g(x), then if y is substituted for x, the rhs of g(x) reduces to y!=y, or false.
  // The correct answer however is true, as f states taht for every boolean y there is an y' that
  // is not equal to it.
  data_specification specification = parse_data_specification(
                    "map f:Bool;\n"
                    "    g:Bool->Bool;\n"
                    "var x:Bool;\n"
                    "eqn f=forall y:Bool.g(y);\n"
                    "    g(x)=exists y:Bool.x!=y;\n");

  rewriter r(specification, s);

  data_expression t0a=parse_data_expression("f",specification);
  data_expression t0true=parse_data_expression("true",specification);
  BOOST_CHECK(r(t0a) == r(t0true));
}


int test_main(int argc, char** argv)
{
  rewrite_strategy_vector strategies(utilities::get_test_rewrite_strategies(false));
  for (rewrite_strategy_vector::const_iterator strat = strategies.begin(); strat != strategies.end(); ++strat)
  {
    std::clog << "  Strategy: " << *strat << std::endl;
    quantifier_expression_test(*strat);
    quantifier_in_rewrite_rules_test(*strat);
  }

  return EXIT_SUCCESS;
}
