// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/forall.h"
#include "mcrl2/data/function_update.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/utilities/test_utilities.h"

using mcrl2::utilities::collect_after_test_case;
using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::sort_bool;
using namespace mcrl2::data::sort_nat;

BOOST_GLOBAL_FIXTURE(collect_after_test_case)

template <typename T>
void test_term(const std::string& s, const T& x)
{
  std::cout << data::pp(x) << std::endl;
}

void test_term(const std::string& s)
{
  atermpp::aterm a = atermpp::read_term_from_string(s);
  if (s.find("DataEqn") == 0)
  {
    data_equation x (a);
    test_term(s, x);
  }
  else if (s.find("SortCons") == 0)
  {
    sort_expression x (a);
    test_term(s, x);
  }
  else if (s.find("OpId") == 0)
  {
    data::function_symbol x (a);
    test_term(s, x);
  }
  else
  {
    data_expression x = atermpp::down_cast<data_expression>(a);
    test_term(s, x);
  }
}

BOOST_AUTO_TEST_CASE(problem_cases)
{
/*
  test_term("DataAppl(OpId(\"@set\",SortArrow([SortArrow([SortId(\"Pos\")],SortId(\"Bool\")),SortCons(SortFSet,SortId(\"Pos\"))],SortCons(SortSet,SortId(\"Pos\")))),OpId(\"@false_\",SortArrow([SortId(\"Pos\")],SortId(\"Bool\"))),DataAppl(OpId(\"@fset_cons\",SortArrow([SortId(\"Pos\"),SortCons(SortFSet,SortId(\"Pos\"))],SortCons(SortFSet,SortId(\"Pos\")))),OpId(\"@c1\",SortId(\"Pos\")),OpId(\"@fset_empty\",SortCons(SortFSet,SortId(\"Pos\")))))");

  test_term("DataAppl(OpId(\"@bag\",SortArrow([SortArrow([SortId(\"ActionLabel\")],SortId(\"Nat\")),SortCons(SortFBag,SortId(\"ActionLabel\"))],SortCons(SortBag,SortId(\"ActionLabel\")))),OpId(\"@zero_\",SortArrow([SortId(\"ActionLabel\")],SortId(\"Nat\"))),DataAppl(OpId(\"@fbag_cons\",SortArrow([SortId(\"ActionLabel\"),SortId(\"Pos\"),SortCons(SortFBag,SortId(\"ActionLabel\"))],SortCons(SortFBag,SortId(\"ActionLabel\")))),OpId(\"a1\",SortId(\"ActionLabel\")),OpId(\"@c1\",SortId(\"Pos\")),OpId(\"@fbag_empty\",SortCons(SortFBag,SortId(\"ActionLabel\")))))");

  test_term("DataEqn([DataVarId(\"X0\",SortCons(SortSet,SortId(\"Bool\"))),DataVarId(\"X1\",SortCons(SortSet,SortId(\"Bool\")))],OpId(\"true\",SortId(\"Bool\")),DataAppl(OpId(\"LiftGenerated_not\",SortArrow([SortCons(SortSet,SortId(\"Bool\"))],SortCons(SortSet,SortId(\"Bool\")))),DataVarId(\"X0\",SortCons(SortSet,SortId(\"Bool\")))),DataAppl(OpId(\"@set\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Bool\")),SortCons(SortFSet,SortId(\"Bool\"))],SortCons(SortSet,SortId(\"Bool\")))),DataAppl(OpId(\"lambda@1\",SortArrow([SortCons(SortSet,SortId(\"Bool\"))],SortArrow([SortId(\"Bool\")],SortId(\"Bool\")))),DataVarId(\"X0\",SortCons(SortSet,SortId(\"Bool\")))),OpId(\"@sort_fset::empty\",SortCons(SortFSet,SortId(\"Bool\")))))");

  test_term("DataAppl(OpId(\"==\",SortArrow([SortId(\"Int\"),SortId(\"Int\")],SortId(\"Bool\"))),DataAppl(OpId(\"*\",SortArrow([SortId(\"Int\"),SortId(\"Int\")],SortId(\"Int\"))),DataVarId(\"x\",SortId(\"Int\")),DataAppl(OpId(\"@cInt\",SortArrow([SortId(\"Nat\")],SortId(\"Int\"))),DataAppl(OpId(\"@cNat\",SortArrow([SortId(\"Pos\")],SortId(\"Nat\"))),DataVarId(\"q\",SortId(\"Pos\"))))),DataAppl(OpId(\"*\",SortArrow([SortId(\"Int\"),SortId(\"Int\")],SortId(\"Int\"))),DataVarId(\"y\",SortId(\"Int\")),DataAppl(OpId(\"@cInt\",SortArrow([SortId(\"Nat\")],SortId(\"Int\"))),DataAppl(OpId(\"@cNat\",SortArrow([SortId(\"Pos\")],SortId(\"Nat\"))),DataVarId(\"p\",SortId(\"Pos\"))))))");

  test_term("DataAppl(OpId(\"@bag\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Nat\")),SortCons(SortFBag,SortId(\"Bool\"))],SortCons(SortBag,SortId(\"Bool\")))),DataAppl(OpId(\"@add_\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Nat\")),SortArrow([SortId(\"Bool\")],SortId(\"Nat\"))],SortArrow([SortId(\"Bool\")],SortId(\"Nat\")))),DataVarId(\"f\",SortArrow([SortId(\"Bool\")],SortId(\"Nat\"))),DataVarId(\"g\",SortArrow([SortId(\"Bool\")],SortId(\"Nat\")))),DataAppl(OpId(\"@fbag_join\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Nat\")),SortArrow([SortId(\"Bool\")],SortId(\"Nat\")),SortCons(SortFBag,SortId(\"Bool\")),SortCons(SortFBag,SortId(\"Bool\"))],SortCons(SortFBag,SortId(\"Bool\")))),DataVarId(\"f\",SortArrow([SortId(\"Bool\")],SortId(\"Nat\"))),DataVarId(\"g\",SortArrow([SortId(\"Bool\")],SortId(\"Nat\"))),DataVarId(\"b\",SortCons(SortFBag,SortId(\"Bool\"))),DataVarId(\"c\",SortCons(SortFBag,SortId(\"Bool\")))))");

  test_term("DataAppl(OpId(\"@bagcomp\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Nat\"))],SortCons(SortBag,SortId(\"Bool\")))),DataVarId(\"f\",SortArrow([SortId(\"Bool\")],SortId(\"Nat\"))))");
  test_term("DataAppl(OpId(\"==\",SortArrow([SortId(\"Bool\"),SortId(\"Bool\")],SortId(\"Bool\"))),DataVarId(\"b\",SortId(\"Bool\")),DataVarId(\"b\",SortId(\"Bool\")))");
  test_term("OpId(\"e1\",SortId(\"Enum\"))");

  test_term("DataAppl(OpId(\"div\",SortArrow([SortId(\"Pos\"),SortId(\"Pos\")],SortId(\"Nat\"))),OpId(\"@c1\",SortId(\"Pos\")),DataAppl(OpId(\"@cDub\",SortArrow([SortId(\"Bool\"),SortId(\"Pos\")],SortId(\"Pos\"))),DataVarId(\"b\",SortId(\"Bool\")),DataVarId(\"p\",SortId(\"Pos\"))))");
  test_term("DataAppl(OpId(\"@cNeg\",SortArrow([SortId(\"Pos\")],SortId(\"Int\"))),DataAppl(OpId(\"@cDub\",SortArrow([SortId(\"Bool\"),SortId(\"Pos\")],SortId(\"Pos\"))),OpId(\"false\",SortId(\"Bool\")),DataVarId(\"p\",SortId(\"Pos\"))))");
  test_term("DataAppl(OpId(\"-\",SortArrow([SortId(\"Real\")],SortId(\"Real\"))),DataAppl(OpId(\"@cReal\",SortArrow([SortId(\"Int\"),SortId(\"Pos\")],SortId(\"Real\"))),DataVarId(\"x\",SortId(\"Int\")),DataVarId(\"p\",SortId(\"Pos\"))))");
  test_term("DataAppl(OpId(\"@cReal\",SortArrow([SortId(\"Int\"),SortId(\"Pos\")],SortId(\"Real\"))),DataAppl(OpId(\"-\",SortArrow([SortId(\"Int\")],SortId(\"Int\"))),DataVarId(\"x\",SortId(\"Int\"))),DataVarId(\"p\",SortId(\"Pos\")))");
  test_term("DataAppl(OpId(\"@set\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Bool\")),SortCons(SortFSet,SortId(\"Bool\"))],SortCons(SortSet,SortId(\"Bool\")))),OpId(\"@false_\",SortArrow([SortId(\"Bool\")],SortId(\"Bool\"))),OpId(\"@sort_fset::empty\",SortCons(SortFSet,SortId(\"Bool\"))))");
  test_term("OpId(\"@c0\",SortId(\"Nat\"))");
  test_term("DataAppl(OpId(\"@set\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Bool\")),SortCons(SortFSet,SortId(\"Bool\"))],SortCons(SortSet,SortId(\"Bool\")))),OpId(\"@false_\",SortArrow([SortId(\"Bool\")],SortId(\"Bool\"))),OpId(\"@sort_fset::empty\",SortCons(SortFSet,SortId(\"Bool\"))))");

  test_term("variable_list()", variable_list());

  test_term("DataAppl(OpId(\"@bag\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Nat\")),SortCons(SortFBag,SortId(\"Bool\"))],SortCons(SortBag,SortId(\"Bool\")))),DataVarId(\"f\",SortArrow([SortId(\"Bool\")],SortId(\"Nat\"))),OpId(\"@fbag_empty\",SortCons(SortFBag,SortId(\"Bool\"))))");

  test_term("DataAppl(OpId(\"mod\",SortArrow([SortId(\"Int\"),SortId(\"Pos\")],SortId(\"Nat\"))),DataVarId(\"y\",SortId(\"Int\")),DataVarId(\"q\",SortId(\"Pos\")))");

  test_term("SortCons(SortFSet,SortId(\"Bool\"))");
  test_term("SortCons(SortFBag,SortId(\"Bool\"))");

  test_term("DataAppl(OpId(\"@set\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Bool\")),SortCons(SortFSet,SortId(\"Bool\"))],SortCons(SortSet,SortId(\"Bool\")))),OpId(\"@false_\",SortArrow([SortId(\"Bool\")],SortId(\"Bool\"))),DataVarId(\"s\",SortCons(SortFSet,SortId(\"Bool\"))))");
  test_term("DataAppl(OpId(\"@set\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Bool\")),SortCons(SortFSet,SortId(\"Bool\"))],SortCons(SortSet,SortId(\"Bool\")))),DataVarId(\"f\",SortArrow([SortId(\"Bool\")],SortId(\"Bool\"))),OpId(\"@sort_fset::empty\",SortCons(SortFSet,SortId(\"Bool\"))))");
  test_term("OpId(\"@c0\",SortId(\"Nat\"))");
  test_term("DataEqn([DataVarId(\"n\",SortId(\"Nat\"))],OpId(\"true\",SortId(\"Bool\")),DataAppl(OpId(\"exp\",SortArrow([SortId(\"Nat\"),SortId(\"Nat\")],SortId(\"Nat\"))),DataVarId(\"n\",SortId(\"Nat\")),OpId(\"@c0\",SortId(\"Nat\"))),DataAppl(OpId(\"@cNat\",SortArrow([SortId(\"Pos\")],SortId(\"Nat\"))),OpId(\"@c1\",SortId(\"Pos\"))))");
  test_term("DataEqn([DataVarId(\"p\",SortId(\"Pos\"))],OpId(\"true\",SortId(\"Bool\")),DataAppl(OpId(\"@dub\",SortArrow([SortId(\"Bool\"),SortId(\"Int\")],SortId(\"Int\"))),OpId(\"false\",SortId(\"Bool\")),DataAppl(OpId(\"@cNeg\",SortArrow([SortId(\"Pos\")],SortId(\"Int\"))),DataVarId(\"p\",SortId(\"Pos\")))),DataAppl(OpId(\"@cNeg\",SortArrow([SortId(\"Pos\")],SortId(\"Int\"))),DataAppl(OpId(\"@cDub\",SortArrow([SortId(\"Bool\"),SortId(\"Pos\")],SortId(\"Pos\"))),OpId(\"false\",SortId(\"Bool\")),DataVarId(\"p\",SortId(\"Pos\")))))");
  test_term("DataEqn([DataVarId(\"p\",SortId(\"Pos\")),DataVarId(\"x\",SortId(\"Int\"))],OpId(\"true\",SortId(\"Bool\")),DataAppl(OpId(\"-\",SortArrow([SortId(\"Real\")],SortId(\"Real\"))),DataAppl(OpId(\"@cReal\",SortArrow([SortId(\"Int\"),SortId(\"Pos\")],SortId(\"Real\"))),DataVarId(\"x\",SortId(\"Int\")),DataVarId(\"p\",SortId(\"Pos\")))),DataAppl(OpId(\"@cReal\",SortArrow([SortId(\"Int\"),SortId(\"Pos\")],SortId(\"Real\"))),DataAppl(OpId(\"-\",SortArrow([SortId(\"Int\")],SortId(\"Int\"))),DataVarId(\"x\",SortId(\"Int\"))),DataVarId(\"p\",SortId(\"Pos\"))))");

  test_term("DataAppl(OpId(\"@bag\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Nat\")),SortCons(SortFBag,SortId(\"Bool\"))],SortCons(SortBag,SortId(\"Bool\")))),OpId(\"@zero_\",SortArrow([SortId(\"Bool\")],SortId(\"Nat\"))),DataVarId(\"b\",SortCons(SortFBag,SortId(\"Bool\"))))");

  // <pp>   @bagfbag(b)  =  @bagfbag(b)
  // <print>b  =  {b}
  // <aterm>DataEqn([DataVarId("b",SortCons(SortFBag,SortId("Bool")))],OpId("true",SortId("Bool")),DataAppl(OpId("@bagfbag",SortArrow([SortCons(SortFBag,SortId("Bool"))],SortCons(SortBag,SortId("Bool")))),[DataVarId("b",SortCons(SortFBag,SortId("Bool")))]),DataAppl(OpId("@bag",SortArrow([SortArrow([SortId("Bool")],SortId("Nat")),SortCons(SortFBag,SortId("Bool"))],SortCons(SortBag,SortId("Bool")))),[OpId("@zero_",SortArrow([SortId("Bool")],SortId("Nat"))),DataVarId("b",SortCons(SortFBag,SortId("Bool")))]))
  test_term("DataEqn([DataVarId(\"b\",SortCons(SortFBag,SortId(\"Bool\")))],OpId(\"true\",SortId(\"Bool\")),DataAppl(OpId(\"@bagfbag\",SortArrow([SortCons(SortFBag,SortId(\"Bool\"))],SortCons(SortBag,SortId(\"Bool\")))),DataVarId(\"b\",SortCons(SortFBag,SortId(\"Bool\")))),DataAppl(OpId(\"@bag\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Nat\")),SortCons(SortFBag,SortId(\"Bool\"))],SortCons(SortBag,SortId(\"Bool\")))),OpId(\"@zero_\",SortArrow([SortId(\"Bool\")],SortId(\"Nat\"))),DataVarId(\"b\",SortCons(SortFBag,SortId(\"Bool\")))))");

  test_term("DataAppl(OpId(\"@bag\",SortArrow([SortArrow([SortId(\"Nat\")],SortId(\"Nat\")),SortCons(SortFBag,SortId(\"Nat\"))],SortCons(SortBag,SortId(\"Nat\")))),Binder(Lambda,[DataVarId(\"x\",SortId(\"Nat\"))],DataVarId(\"x\",SortId(\"Nat\"))),OpId(\"@fbag_empty\",SortCons(SortFBag,SortId(\"Nat\"))))");
  // <pp>   { b: Bool | b }
  // <print>{ [b: Bool] | b }
  // <aterm>DataAppl(OpId("@set",SortArrow([SortArrow([SortId("Bool")],SortId("Bool")),SortCons(SortFSet,SortId("Bool"))],SortCons(SortSet,SortId("Bool")))),[Binder(Lambda,[DataVarId("b",SortId("Bool"))],DataVarId("b",SortId("Bool"))),OpId("@sort_fset::empty",SortCons(SortFSet,SortId("Bool")))])

  test_term("DataAppl(OpId(\"@set\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Bool\")),SortCons(SortFSet,SortId(\"Bool\"))],SortCons(SortSet,SortId(\"Bool\")))),Binder(Lambda,[DataVarId(\"b\",SortId(\"Bool\"))],DataVarId(\"b\",SortId(\"Bool\"))),OpId(\"@sort_fset::empty\",SortCons(SortFSet,SortId(\"Bool\"))))");

  // <pp>   { b: Bool | if(b, 2, 3) }
  // <print>{ [b: Bool] | if(b, 2, 3) }
  // <aterm>DataAppl(OpId("@bag",SortArrow([SortArrow([SortId("Bool")],SortId("Nat")),SortCons(SortFBag,SortId("Bool"))],SortCons(SortBag,SortId("Bool")))),[Binder(Lambda,[DataVarId("b",SortId("Bool"))],DataAppl(OpId("if",SortArrow([SortId("Bool"),SortId("Nat"),SortId("Nat")],SortId("Nat"))),[DataVarId("b",SortId("Bool")),DataAppl(OpId("Pos2Nat",SortArrow([SortId("Pos")],SortId("Nat"))),[DataAppl(OpId("@cDub",SortArrow([SortId("Bool"),SortId("Pos")],SortId("Pos"))),[OpId("false",SortId("Bool")),OpId("@c1",SortId("Pos"))])]),DataAppl(OpId("Pos2Nat",SortArrow([SortId("Pos")],SortId("Nat"))),[DataAppl(OpId("@cDub",SortArrow([SortId("Bool"),SortId("Pos")],SortId("Pos"))),[OpId("true",SortId("Bool")),OpId("@c1",SortId("Pos"))])])])),OpId("@fbag_empty",SortCons(SortFBag,SortId("Bool")))])
  test_term("DataAppl(OpId(\"@bag\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Nat\")),SortCons(SortFBag,SortId(\"Bool\"))],SortCons(SortBag,SortId(\"Bool\")))),Binder(Lambda,[DataVarId(\"b\",SortId(\"Bool\"))],DataAppl(OpId(\"if\",SortArrow([SortId(\"Bool\"),SortId(\"Nat\"),SortId(\"Nat\")],SortId(\"Nat\"))),DataVarId(\"b\",SortId(\"Bool\")),DataAppl(OpId(\"Pos2Nat\",SortArrow([SortId(\"Pos\")],SortId(\"Nat\"))),DataAppl(OpId(\"@cDub\",SortArrow([SortId(\"Bool\"),SortId(\"Pos\")],SortId(\"Pos\"))),OpId(\"false\",SortId(\"Bool\")),OpId(\"@c1\",SortId(\"Pos\")))),DataAppl(OpId(\"Pos2Nat\",SortArrow([SortId(\"Pos\")],SortId(\"Nat\"))),DataAppl(OpId(\"@cDub\",SortArrow([SortId(\"Bool\"),SortId(\"Pos\")],SortId(\"Pos\"))),OpId(\"true\",SortId(\"Bool\")),OpId(\"@c1\",SortId(\"Pos\")))))),OpId(\"@fbag_empty\",SortCons(SortFBag,SortId(\"Bool\"))))");

  // <pp>   !true  =  false, !false  =  true, !!b  =  b, b && true  =  b, b && false  =  false, true && b  =  b, false && b  =  false, b || true  =  true, b || false  =  b, true || b  =  true, false || b  =  b, b => true  =  true, b => false  =  !b, true => b  =  b, false => b  =  true, true == b  =  b, false == b  =  !b, b == true  =  b, b == false  =  !b, false < b  =  b, true < b  =  false, b < false  =  false, b < true  =  !b, false <= b  =  true, true <= b  =  b, b <= false  =  !b, b <= true  =  true
  // <print>!true  =  false!false  =  true!!b  =  bb && true  =  bb && false  =  falsetrue && b  =  bfalse && b  =  falseb || true  =  trueb || false  =  btrue || b  =  truefalse || b  =  bb => true  =  trueb => false  =  !btrue => b  =  bfalse => b  =  truetrue == b  =  bfalse == b  =  !bb == true  =  bb == false  =  !bfalse < b  =  btrue < b  =  falseb < false  =  falseb < true  =  !bfalse <= b  =  truetrue <= b  =  bb <= false  =  !bb <= true  =  true
  // <aterm>unknown

  // <pp>   1 == 2 * p + 2 * if(b, 1, 0)  =  false
  // <print>1 == @cDub(b, p)  =  false
  // <aterm>DataEqn([DataVarId("b",SortId("Bool")),DataVarId("p",SortId("Pos"))],OpId("true",SortId("Bool")),DataAppl(OpId("==",SortArrow([SortId("Pos"),SortId("Pos")],SortId("Bool"))),[OpId("@c1",SortId("Pos")),DataAppl(OpId("@cDub",SortArrow([SortId("Bool"),SortId("Pos")],SortId("Pos"))),[DataVarId("b",SortId("Bool")),DataVarId("p",SortId("Pos"))])]),OpId("false",SortId("Bool")))
  test_term("DataEqn([DataVarId(\"b\",SortId(\"Bool\")),DataVarId(\"p\",SortId(\"Pos\"))],OpId(\"true\",SortId(\"Bool\")),DataAppl(OpId(\"==\",SortArrow([SortId(\"Pos\"),SortId(\"Pos\")],SortId(\"Bool\"))),OpId(\"@c1\",SortId(\"Pos\")),DataAppl(OpId(\"@cDub\",SortArrow([SortId(\"Bool\"),SortId(\"Pos\")],SortId(\"Pos\"))),DataVarId(\"b\",SortId(\"Bool\")),DataVarId(\"p\",SortId(\"Pos\")))),OpId(\"false\",SortId(\"Bool\")))");

  test_term("DataAppl(OpId(\"mod\",SortArrow([SortId(\"Nat\"),SortId(\"Pos\")],SortId(\"Nat\"))),DataVarId(\"n\",SortId(\"Nat\")),DataAppl(OpId(\"@cDub\",SortArrow([SortId(\"Bool\"),SortId(\"Pos\")],SortId(\"Pos\"))),OpId(\"false\",SortId(\"Bool\")),OpId(\"@c1\",SortId(\"Pos\"))))");

  // <pp>   succ(succ(n))  =  2 * succ(n div 2) + 2 * if(n mod 2 == 1, 1, 0)
  // <print>succ(succ(n))  =  @cDub(n mod 2 == 1, succ(n / 2))
  // <aterm>DataEqn([DataVarId("n",SortId("Nat"))],OpId("true",SortId("Bool")),DataAppl(OpId("succ",SortArrow([SortId("Pos")],SortId("Pos"))),[DataAppl(OpId("succ",SortArrow([SortId("Nat")],SortId("Pos"))),[DataVarId("n",SortId("Nat"))])]),DataAppl(OpId("@cDub",SortArrow([SortId("Bool"),SortId("Pos")],SortId("Pos"))),[DataAppl(OpId("==",SortArrow([SortId("Nat"),SortId("Nat")],SortId("Bool"))),[DataAppl(OpId("mod",SortArrow([SortId("Nat"),SortId("Pos")],SortId("Nat"))),[DataVarId("n",SortId("Nat")),DataAppl(OpId("@cDub",SortArrow([SortId("Bool"),SortId("Pos")],SortId("Pos"))),[OpId("false",SortId("Bool")),OpId("@c1",SortId("Pos"))])]),DataAppl(OpId("@cNat",SortArrow([SortId("Pos")],SortId("Nat"))),[OpId("@c1",SortId("Pos"))])]),DataAppl(OpId("succ",SortArrow([SortId("Nat")],SortId("Pos"))),[DataAppl(OpId("div",SortArrow([SortId("Nat"),SortId("Pos")],SortId("Nat"))),[DataVarId("n",SortId("Nat")),DataAppl(OpId("@cDub",SortArrow([SortId("Bool"),SortId("Pos")],SortId("Pos"))),[OpId("false",SortId("Bool")),OpId("@c1",SortId("Pos"))])])])]))
  test_term("DataEqn([DataVarId(\"n\",SortId(\"Nat\"))],OpId(\"true\",SortId(\"Bool\")),DataAppl(OpId(\"succ\",SortArrow([SortId(\"Pos\")],SortId(\"Pos\"))),DataAppl(OpId(\"succ\",SortArrow([SortId(\"Nat\")],SortId(\"Pos\"))),DataVarId(\"n\",SortId(\"Nat\")))),DataAppl(OpId(\"@cDub\",SortArrow([SortId(\"Bool\"),SortId(\"Pos\")],SortId(\"Pos\"))),DataAppl(OpId(\"==\",SortArrow([SortId(\"Nat\"),SortId(\"Nat\")],SortId(\"Bool\"))),DataAppl(OpId(\"mod\",SortArrow([SortId(\"Nat\"),SortId(\"Pos\")],SortId(\"Nat\"))),DataVarId(\"n\",SortId(\"Nat\")),DataAppl(OpId(\"@cDub\",SortArrow([SortId(\"Bool\"),SortId(\"Pos\")],SortId(\"Pos\"))),OpId(\"false\",SortId(\"Bool\")),OpId(\"@c1\",SortId(\"Pos\")))),DataAppl(OpId(\"@cNat\",SortArrow([SortId(\"Pos\")],SortId(\"Nat\"))),OpId(\"@c1\",SortId(\"Pos\")))),DataAppl(OpId(\"succ\",SortArrow([SortId(\"Nat\")],SortId(\"Pos\"))),DataAppl(OpId(\"div\",SortArrow([SortId(\"Nat\"),SortId(\"Pos\")],SortId(\"Nat\"))),DataVarId(\"n\",SortId(\"Nat\")),DataAppl(OpId(\"@cDub\",SortArrow([SortId(\"Bool\"),SortId(\"Pos\")],SortId(\"Pos\"))),OpId(\"false\",SortId(\"Bool\")),OpId(\"@c1\",SortId(\"Pos\")))))))");

  // <pp>   n == -p  =  false
  // <print>n == @cNeg(p)  =  false
  // <aterm>DataEqn([DataVarId("n",SortId("Nat")),DataVarId("p",SortId("Pos"))],OpId("true",SortId("Bool")),DataAppl(OpId("==",SortArrow([SortId("Int"),SortId("Int")],SortId("Bool"))),[DataAppl(OpId("@cInt",SortArrow([SortId("Nat")],SortId("Int"))),[DataVarId("n",SortId("Nat"))]),DataAppl(OpId("@cNeg",SortArrow([SortId("Pos")],SortId("Int"))),[DataVarId("p",SortId("Pos"))])]),OpId("false",SortId("Bool")))
  test_term("DataEqn([DataVarId(\"n\",SortId(\"Nat\")),DataVarId(\"p\",SortId(\"Pos\"))],OpId(\"true\",SortId(\"Bool\")),DataAppl(OpId(\"==\",SortArrow([SortId(\"Int\"),SortId(\"Int\")],SortId(\"Bool\"))),DataAppl(OpId(\"@cInt\",SortArrow([SortId(\"Nat\")],SortId(\"Int\"))),DataVarId(\"n\",SortId(\"Nat\"))),DataAppl(OpId(\"@cNeg\",SortArrow([SortId(\"Pos\")],SortId(\"Int\"))),DataVarId(\"p\",SortId(\"Pos\")))),OpId(\"false\",SortId(\"Bool\")))");

  // <pp>   x / p == y / q  =  x * q == y * p
  // <print>x == y  =  x * q == y * p
  // <aterm>DataEqn([DataVarId("p",SortId("Pos")),DataVarId("q",SortId("Pos")),DataVarId("x",SortId("Int")),DataVarId("y",SortId("Int"))],OpId("true",SortId("Bool")),DataAppl(OpId("==",SortArrow([SortId("Real"),SortId("Real")],SortId("Bool"))),[DataAppl(OpId("@cReal",SortArrow([SortId("Int"),SortId("Pos")],SortId("Real"))),[DataVarId("x",SortId("Int")),DataVarId("p",SortId("Pos"))]),DataAppl(OpId("@cReal",SortArrow([SortId("Int"),SortId("Pos")],SortId("Real"))),[DataVarId("y",SortId("Int")),DataVarId("q",SortId("Pos"))])]),DataAppl(OpId("==",SortArrow([SortId("Int"),SortId("Int")],SortId("Bool"))),[DataAppl(OpId("*",SortArrow([SortId("Int"),SortId("Int")],SortId("Int"))),[DataVarId("x",SortId("Int")),DataAppl(OpId("@cInt",SortArrow([SortId("Nat")],SortId("Int"))),[DataAppl(OpId("@cNat",SortArrow([SortId("Pos")],SortId("Nat"))),[DataVarId("q",SortId("Pos"))])])]),DataAppl(OpId("*",SortArrow([SortId("Int"),SortId("Int")],SortId("Int"))),[DataVarId("y",SortId("Int")),DataAppl(OpId("@cInt",SortArrow([SortId("Nat")],SortId("Int"))),[DataAppl(OpId("@cNat",SortArrow([SortId("Pos")],SortId("Nat"))),[DataVarId("p",SortId("Pos"))])])])]))
  test_term("DataEqn([DataVarId(\"p\",SortId(\"Pos\")),DataVarId(\"q\",SortId(\"Pos\")),DataVarId(\"x\",SortId(\"Int\")),DataVarId(\"y\",SortId(\"Int\"))],OpId(\"true\",SortId(\"Bool\")),DataAppl(OpId(\"==\",SortArrow([SortId(\"Real\"),SortId(\"Real\")],SortId(\"Bool\"))),DataAppl(OpId(\"@cReal\",SortArrow([SortId(\"Int\"),SortId(\"Pos\")],SortId(\"Real\"))),DataVarId(\"x\",SortId(\"Int\")),DataVarId(\"p\",SortId(\"Pos\"))),DataAppl(OpId(\"@cReal\",SortArrow([SortId(\"Int\"),SortId(\"Pos\")],SortId(\"Real\"))),DataVarId(\"y\",SortId(\"Int\")),DataVarId(\"q\",SortId(\"Pos\")))),DataAppl(OpId(\"==\",SortArrow([SortId(\"Int\"),SortId(\"Int\")],SortId(\"Bool\"))),DataAppl(OpId(\"*\",SortArrow([SortId(\"Int\"),SortId(\"Int\")],SortId(\"Int\"))),DataVarId(\"x\",SortId(\"Int\")),DataAppl(OpId(\"@cInt\",SortArrow([SortId(\"Nat\")],SortId(\"Int\"))),DataAppl(OpId(\"@cNat\",SortArrow([SortId(\"Pos\")],SortId(\"Nat\"))),DataVarId(\"q\",SortId(\"Pos\"))))),DataAppl(OpId(\"*\",SortArrow([SortId(\"Int\"),SortId(\"Int\")],SortId(\"Int\"))),DataVarId(\"y\",SortId(\"Int\")),DataAppl(OpId(\"@cInt\",SortArrow([SortId(\"Nat\")],SortId(\"Int\"))),DataAppl(OpId(\"@cNat\",SortArrow([SortId(\"Pos\")],SortId(\"Nat\"))),DataVarId(\"p\",SortId(\"Pos\")))))))");

  test_term("OpId(\"@sort_fset::empty\",SortCons(SortFSet,SortId(\"Bool\")))");

  // <pp>   {} == {}  =  true
  // <print>@sort_fset::empty == @sort_fset::empty  =  true
  // <aterm>DataEqn([],OpId("true",SortId("Bool")),DataAppl(OpId("==",SortArrow([SortCons(SortFSet,SortId("Bool")),SortCons(SortFSet,SortId("Bool"))],SortId("Bool"))),[OpId("@sort_fset::empty",SortCons(SortFSet,SortId("Bool"))),OpId("@sort_fset::empty",SortCons(SortFSet,SortId("Bool")))]),OpId("true",SortId("Bool")))
  test_term("DataEqn([],OpId(\"true\",SortId(\"Bool\")),DataAppl(OpId(\"==\",SortArrow([SortCons(SortFSet,SortId(\"Bool\")),SortCons(SortFSet,SortId(\"Bool\"))],SortId(\"Bool\"))),OpId(\"@sort_fset::empty\",SortCons(SortFSet,SortId(\"Bool\"))),OpId(\"@sort_fset::empty\",SortCons(SortFSet,SortId(\"Bool\")))),OpId(\"true\",SortId(\"Bool\")))");

  test_term("DataAppl(OpId(\"@sort_set::set_fset\",SortArrow([SortCons(SortFSet,SortId(\"Bool\"))],SortCons(SortSet,SortId(\"Bool\")))),DataVarId(\"s\",SortCons(SortFSet,SortId(\"Bool\"))))");

  // <pp>   @sort_set::set_fset(s)  =  @sort_set::set_fset(s)
  // <print>s  =  { x: Bool | @false_(x) != x in s }
  // <aterm>DataEqn([DataVarId("s",SortCons(SortFSet,SortId("Bool")))],OpId("true",SortId("Bool")),DataAppl(OpId("@sort_set::set_fset",SortArrow([SortCons(SortFSet,SortId("Bool"))],SortCons(SortSet,SortId("Bool")))),[DataVarId("s",SortCons(SortFSet,SortId("Bool")))]),DataAppl(OpId("@set",SortArrow([SortArrow([SortId("Bool")],SortId("Bool")),SortCons(SortFSet,SortId("Bool"))],SortCons(SortSet,SortId("Bool")))),[OpId("@false_",SortArrow([SortId("Bool")],SortId("Bool"))),DataVarId("s",SortCons(SortFSet,SortId("Bool")))]))
  test_term("DataEqn([DataVarId(\"s\",SortCons(SortFSet,SortId(\"Bool\")))],OpId(\"true\",SortId(\"Bool\")),DataAppl(OpId(\"@sort_set::set_fset\",SortArrow([SortCons(SortFSet,SortId(\"Bool\"))],SortCons(SortSet,SortId(\"Bool\")))),DataVarId(\"s\",SortCons(SortFSet,SortId(\"Bool\")))),DataAppl(OpId(\"@set\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Bool\")),SortCons(SortFSet,SortId(\"Bool\"))],SortCons(SortSet,SortId(\"Bool\")))),OpId(\"@false_\",SortArrow([SortId(\"Bool\")],SortId(\"Bool\"))),DataVarId(\"s\",SortCons(SortFSet,SortId(\"Bool\")))))");

  // <pp>   @fbag_cons(v0, v1, v2) < @fbag_cons(v3, v4, v5)  =  v0 < v3 || v0 == v3 && v1 < v4 || v1 == v4 && v2 < v5
  // <print>@fbag_cons(v0, v1, v2) < @fbag_cons(v3, v4, v5)  =  v0 < v3 || (v0 == v3 && (v1 < v4 || (v1 == v4 && v2 < v5)))
  // <aterm>DataEqn([DataVarId("v0",SortId("Bool")),DataVarId("v1",SortId("Pos")),DataVarId("v2",SortCons(SortFBag,SortId("Bool"))),DataVarId("v3",SortId("Bool")),DataVarId("v4",SortId("Pos")),DataVarId("v5",SortCons(SortFBag,SortId("Bool")))],OpId("true",SortId("Bool")),DataAppl(OpId("<",SortArrow([SortCons(SortFBag,SortId("Bool")),SortCons(SortFBag,SortId("Bool"))],SortId("Bool"))),[DataAppl(OpId("@fbag_cons",SortArrow([SortId("Bool"),SortId("Pos"),SortCons(SortFBag,SortId("Bool"))],SortCons(SortFBag,SortId("Bool")))),[DataVarId("v0",SortId("Bool")),DataVarId("v1",SortId("Pos")),DataVarId("v2",SortCons(SortFBag,SortId("Bool")))]),DataAppl(OpId("@fbag_cons",SortArrow([SortId("Bool"),SortId("Pos"),SortCons(SortFBag,SortId("Bool"))],SortCons(SortFBag,SortId("Bool")))),[DataVarId("v3",SortId("Bool")),DataVarId("v4",SortId("Pos")),DataVarId("v5",SortCons(SortFBag,SortId("Bool")))])]),DataAppl(OpId("||",SortArrow([SortId("Bool"),SortId("Bool")],SortId("Bool"))),[DataAppl(OpId("<",SortArrow([SortId("Bool"),SortId("Bool")],SortId("Bool"))),[DataVarId("v0",SortId("Bool")),DataVarId("v3",SortId("Bool"))]),DataAppl(OpId("&&",SortArrow([SortId("Bool"),SortId("Bool")],SortId("Bool"))),[DataAppl(OpId("==",SortArrow([SortId("Bool"),SortId("Bool")],SortId("Bool"))),[DataVarId("v0",SortId("Bool")),DataVarId("v3",SortId("Bool"))]),DataAppl(OpId("||",SortArrow([SortId("Bool"),SortId("Bool")],SortId("Bool"))),[DataAppl(OpId("<",SortArrow([SortId("Pos"),SortId("Pos")],SortId("Bool"))),[DataVarId("v1",SortId("Pos")),DataVarId("v4",SortId("Pos"))]),DataAppl(OpId("&&",SortArrow([SortId("Bool"),SortId("Bool")],SortId("Bool"))),[DataAppl(OpId("==",SortArrow([SortId("Pos"),SortId("Pos")],SortId("Bool"))),[DataVarId("v1",SortId("Pos")),DataVarId("v4",SortId("Pos"))]),DataAppl(OpId("<",SortArrow([SortCons(SortFBag,SortId("Bool")),SortCons(SortFBag,SortId("Bool"))],SortId("Bool"))),[DataVarId("v2",SortCons(SortFBag,SortId("Bool"))),DataVarId("v5",SortCons(SortFBag,SortId("Bool")))])])])])]))
  test_term("DataEqn([DataVarId(\"v0\",SortId(\"Bool\")),DataVarId(\"v1\",SortId(\"Pos\")),DataVarId(\"v2\",SortCons(SortFBag,SortId(\"Bool\"))),DataVarId(\"v3\",SortId(\"Bool\")),DataVarId(\"v4\",SortId(\"Pos\")),DataVarId(\"v5\",SortCons(SortFBag,SortId(\"Bool\")))],OpId(\"true\",SortId(\"Bool\")),DataAppl(OpId(\"<\",SortArrow([SortCons(SortFBag,SortId(\"Bool\")),SortCons(SortFBag,SortId(\"Bool\"))],SortId(\"Bool\"))),DataAppl(OpId(\"@fbag_cons\",SortArrow([SortId(\"Bool\"),SortId(\"Pos\"),SortCons(SortFBag,SortId(\"Bool\"))],SortCons(SortFBag,SortId(\"Bool\")))),DataVarId(\"v0\",SortId(\"Bool\")),DataVarId(\"v1\",SortId(\"Pos\")),DataVarId(\"v2\",SortCons(SortFBag,SortId(\"Bool\")))),DataAppl(OpId(\"@fbag_cons\",SortArrow([SortId(\"Bool\"),SortId(\"Pos\"),SortCons(SortFBag,SortId(\"Bool\"))],SortCons(SortFBag,SortId(\"Bool\")))),DataVarId(\"v3\",SortId(\"Bool\")),DataVarId(\"v4\",SortId(\"Pos\")),DataVarId(\"v5\",SortCons(SortFBag,SortId(\"Bool\"))))),DataAppl(OpId(\"||\",SortArrow([SortId(\"Bool\"),SortId(\"Bool\")],SortId(\"Bool\"))),DataAppl(OpId(\"<\",SortArrow([SortId(\"Bool\"),SortId(\"Bool\")],SortId(\"Bool\"))),DataVarId(\"v0\",SortId(\"Bool\")),DataVarId(\"v3\",SortId(\"Bool\"))),DataAppl(OpId(\"&&\",SortArrow([SortId(\"Bool\"),SortId(\"Bool\")],SortId(\"Bool\"))),DataAppl(OpId(\"==\",SortArrow([SortId(\"Bool\"),SortId(\"Bool\")],SortId(\"Bool\"))),DataVarId(\"v0\",SortId(\"Bool\")),DataVarId(\"v3\",SortId(\"Bool\"))),DataAppl(OpId(\"||\",SortArrow([SortId(\"Bool\"),SortId(\"Bool\")],SortId(\"Bool\"))),DataAppl(OpId(\"<\",SortArrow([SortId(\"Pos\"),SortId(\"Pos\")],SortId(\"Bool\"))),DataVarId(\"v1\",SortId(\"Pos\")),DataVarId(\"v4\",SortId(\"Pos\"))),DataAppl(OpId(\"&&\",SortArrow([SortId(\"Bool\"),SortId(\"Bool\")],SortId(\"Bool\"))),DataAppl(OpId(\"==\",SortArrow([SortId(\"Pos\"),SortId(\"Pos\")],SortId(\"Bool\"))),DataVarId(\"v1\",SortId(\"Pos\")),DataVarId(\"v4\",SortId(\"Pos\"))),DataAppl(OpId(\"<\",SortArrow([SortCons(SortFBag,SortId(\"Bool\")),SortCons(SortFBag,SortId(\"Bool\"))],SortId(\"Bool\"))),DataVarId(\"v2\",SortCons(SortFBag,SortId(\"Bool\"))),DataVarId(\"v5\",SortCons(SortFBag,SortId(\"Bool\")))))))))");

  test_term("DataAppl(OpId(\"@setcomp\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Bool\"))],SortCons(SortSet,SortId(\"Bool\")))),DataVarId(\"f\",SortArrow([SortId(\"Bool\")],SortId(\"Bool\"))))");
  test_term("DataAppl(OpId(\"@set\",SortArrow([SortArrow([SortId(\"Bool\")],SortId(\"Bool\")),SortCons(SortFSet,SortId(\"Bool\"))],SortCons(SortSet,SortId(\"Bool\")))),DataVarId(\"f\",SortArrow([SortId(\"Bool\")],SortId(\"Bool\"))),OpId(\"@sort_fset::empty\",SortCons(SortFSet,SortId(\"Bool\"))))");
*/
}

template <typename ExpressionType>
bool print_check(ExpressionType const& left, std::string const& right)
{
  std::clog << "Checking printing of " << data::pp(left) << " with " << right << std::endl;
  if (pp(left) != right)
  {
    std::clog << "pp(" << left << ") != " << right << std::endl;
    std::clog << left << " != " << right << std::endl;

    return false;
  }

  return true;
}

template <typename Container>
bool print_container_check(Container const& c)
{
  std::string r = data::pp(c);
  if (r == "")
  {
    std::clog << "error printing container" << std::endl;
    return false;
  }

  return true;
}

#define PRINT_CHECK(x,y) BOOST_CHECK(print_check(x,y))

BOOST_AUTO_TEST_CASE(test_function_symbol_print)
{
  data::function_symbol f("f", sort_bool::bool_());

  PRINT_CHECK(f, "f");
}

BOOST_AUTO_TEST_CASE(test_application_print)
{
  data::function_symbol f("f", make_function_sort(bool_(), bool_()));
  data::function_symbol g("g", make_function_sort(bool_(), nat(), bool_()));

  PRINT_CHECK(f(true_()), "f(true)");
  PRINT_CHECK(g(false_(), sort_nat::nat(10)), "g(false, 10)");
  PRINT_CHECK(g(f(true_()), sort_nat::nat(10)), "g(f(true), 10)");
}

BOOST_AUTO_TEST_CASE(test_abstraction_print)
{
  using namespace sort_pos;
  using namespace sort_nat;

  variable_vector x(atermpp::make_vector(variable("x", sort_nat::nat())));
  variable_vector y(atermpp::make_vector(variable("y", sort_pos::pos())));
  variable_vector xy(atermpp::make_vector(x[0], y[0]));

  PRINT_CHECK(lambda(x, equal_to(x[0], nat(10))), "lambda x: Nat. x == 10");
  PRINT_CHECK(lambda(xy, equal_to(xy[0], pos2nat(xy[1]))), "lambda x: Nat, y: Pos. x == y");
  PRINT_CHECK(exists(x, equal_to(x[0], nat(10))), "exists x: Nat. x == 10");
  PRINT_CHECK(exists(xy, equal_to(xy[0], pos2nat(xy[1]))), "exists x: Nat, y: Pos. x == y");
  PRINT_CHECK(forall(x, equal_to(x[0], nat(10))), "forall x: Nat. x == 10");
  PRINT_CHECK(forall(xy, equal_to(xy[0], pos2nat(xy[1]))), "forall x: Nat, y: Pos. x == y");
  PRINT_CHECK(forall(x, exists(y, not_equal_to(xy[0], pos2nat(xy[1])))), "forall x: Nat. exists y: Pos. x != y");
}

BOOST_AUTO_TEST_CASE(test_list_print)
{
  using namespace sort_bool;
  using namespace sort_list;

  data_expression empty_(empty(bool_()));

  // Using all operations
  BOOST_CHECK(print_check(empty(bool_()), "[]"));
  BOOST_CHECK(print_check(cons_(bool_(), true_(), empty_), "[true]"));
  BOOST_CHECK(print_check(cons_(bool_(), false_(), cons_(bool_(), true_(), empty_)), "[false, true]"));
  //BOOST_CHECK(print_check(snoc(bool_(), cons_(bool_(), true_(), empty), false_()), "[true, false]"));
  BOOST_CHECK(print_check(snoc(bool_(), cons_(bool_(), true_(), empty_), false_()), "[true] <| false"));
  BOOST_CHECK(print_check(snoc(bool_(), snoc(bool_(), empty_, true_()), false_()), "[true, false]"));
  BOOST_CHECK(print_check(cons_(bool_(), in(bool_(), false_(), cons_(bool_(), true_(), empty_)), empty_), "[false in [true]]"));
  //BOOST_CHECK(print_check(snoc(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty, true_())), true_()), "[false, true, true]"));
  BOOST_CHECK(print_check(snoc(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty_, true_())), true_()), "(false |> [true]) <| true"));
  //BOOST_CHECK(print_check(in(bool_(), true_(), cons_(bool_(), false_(), snoc(bool_(), empty, true_()))), "true in [false, true]"));
  BOOST_CHECK(print_check(in(bool_(), true_(), cons_(bool_(), false_(), snoc(bool_(), empty_, true_()))), "true in false |> [true]"));
  //BOOST_CHECK(print_check(count(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty, true_()))), "#[false, true]"));
  BOOST_CHECK(print_check(count(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty_, true_()))), "#(false |> [true])"));
  //BOOST_CHECK(print_check(concat(bool_(), cons_(bool_(), true_(), empty), cons_(bool_(), false_(), snoc(bool_(), empty, true_()))), "[true] ++ [false, true]"));
  BOOST_CHECK(print_check(concat(bool_(), cons_(bool_(), true_(), empty_), cons_(bool_(), false_(), snoc(bool_(), empty_, true_()))), "[true] ++ (false |> [true])"));
  //BOOST_CHECK(print_check(element_at(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty, true_())), sort_nat::nat(1)), "[false, true].1"));
  BOOST_CHECK(print_check(element_at(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty_, true_())), sort_nat::nat(1)), "(false |> [true]) . 1"));
  BOOST_CHECK(print_check(head(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty_))), "head([false, true])"));
  BOOST_CHECK(print_check(tail(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty_))), "tail([false, true])"));
  BOOST_CHECK(print_check(rhead(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty_))), "rhead([false, true])"));
  BOOST_CHECK(print_check(rtail(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty_))), "rtail([false, true])"));
  BOOST_CHECK(print_check(cons_(bool_(), true_(), concat(bool_(), cons_(bool_(), true_(), empty_), cons_(bool_(), false_(), empty_))), "true |> [true] ++ [false]"));
  BOOST_CHECK(print_check(snoc(bool_(), concat(bool_(), cons_(bool_(), true_(), empty_), cons_(bool_(), false_(), empty_)), true_()), "[true] ++ [false] <| true"));
  BOOST_CHECK(print_check(cons_(bool_(), false_(), snoc(bool_(), concat(bool_(), cons_(bool_(), true_(), empty_), cons_(bool_(), false_(), empty_)), true_())), "false |> [true] ++ [false] <| true"));

  // lists of lists
  data_expression list_empty = empty(list(bool_()));
  data_expression list_true = cons_(bool_(), true_(), empty_);
  data_expression list_false_true = cons_(bool_(), false_(), cons_(bool_(), true_(), empty_));

  BOOST_CHECK(print_check(empty(list(bool_())), "[]"));
  BOOST_CHECK(print_check(list_true, "[true]"));
  BOOST_CHECK(print_check(cons_(list(bool_()), list_true, list_empty), "[[true]]"));
  BOOST_CHECK(print_check(cons_(list(bool_()), list_true, cons_(list(bool_()), list_false_true, list_empty)), "[[true], [false, true]]"));
  BOOST_CHECK(print_check(snoc(list(bool_()), cons_(list(bool_()), list_false_true, list_empty), list_true), "[[false, true]] <| [true]"));
  BOOST_CHECK(print_check(in(list(bool_()), list_true, cons_(list(bool_()), list_true, list_empty)), "[true] in [[true]]"));
  BOOST_CHECK(print_check(in(list(bool_()), list_true, cons_(list(bool_()), list_true, list_empty)), "[true] in [[true]]"));

  // List enumeration
  data_expression_vector v;
  v.push_back(sort_bool::true_());
  v.push_back(sort_bool::false_());
  v.push_back(sort_bool::true_());
  v.push_back(sort_bool::true_());
  data_expression l1(list_enumeration(list(sort_bool::bool_()), v));
  BOOST_CHECK(print_check(l1, "[true, false, true, true]"));

  data_expression l2(sort_list::list(sort_bool::bool_(), v));
  BOOST_CHECK(print_check(l2, "[true, false, true, true]"));

  // Sort expression
  BOOST_CHECK(print_check(list(bool_()), "List(Bool)"));


}

BOOST_AUTO_TEST_CASE(test_set_print)
{
  using namespace sort_bool;

  data_expression set_empty = sort_fset::empty(bool_());
  data_expression set_false = sort_set::set_fset(bool_(), sort_fset::cons_(bool_(), false_(), sort_fset::empty(bool_())));
  data_expression set_true = sort_set::set_fset(bool_(), sort_fset::cons_(bool_(), true_(), sort_fset::empty(bool_())));

  // Using all operations
  BOOST_CHECK(print_check(sort_fset::empty(bool_()), "{}"));
  BOOST_CHECK(print_check(sort_set::set_fset(bool_(), sort_fset::empty(bool_())), "{}"));
  BOOST_CHECK(print_check(sort_set::constructor(bool_(), sort_set::false_function(bool_()), sort_fset::empty(bool_())), "{}"));
  BOOST_CHECK(print_check(sort_set::in(bool_(), false_(), set_empty), "false in {}"));
  BOOST_CHECK(print_check(sort_set::union_(bool_(), set_false, set_true), "{false} + {true}"));
  BOOST_CHECK(print_check(sort_set::intersection(bool_(), set_false, set_true), "{false} * {true}"));
  BOOST_CHECK(print_check(sort_set::difference(bool_(), set_false, set_true), "{false} - {true}"));
  BOOST_CHECK(print_check(sort_set::complement(bool_(), set_false), "!{false}"));

  // Some parsed expressions
  BOOST_CHECK(print_check(parse_data_expression("{true}"), "{true}"));
  BOOST_CHECK(print_check(parse_data_expression("{true, false}"), "{true, false}"));
  BOOST_CHECK(print_check(parse_data_expression("{ b: Bool | b }"), "{ b: Bool | b }"));
  BOOST_CHECK(print_check(parse_data_expression("{ x: Nat | x == 0 }"), "{ x: Nat | x == 0 }"));
  BOOST_CHECK(print_check(parse_data_expression("{ x: Nat | (lambda y: Nat. y == 0)(x) }"), "{ x: Nat | (lambda y: Nat. y == 0)(x) }"));

  // Some types
  BOOST_CHECK(print_check(sort_fset::fset(bool_()), "FSet(Bool)"));
  BOOST_CHECK(print_check(parse_sort_expression("Set(Nat)"), "Set(Nat)"));
}

BOOST_AUTO_TEST_CASE(test_bag_print)
{
  using namespace sort_bool;
  using namespace sort_nat;

  data_expression bag_empty(sort_fbag::empty(bool_()));
  data_expression fbag_empty_(sort_fbag::empty(bool_()));
  data_expression bag_false = sort_bag::bag_fbag(bool_(), sort_fbag::cons_(bool_(), false_(), number(sort_pos::pos(), "1"), fbag_empty_));
  data_expression bag_true = sort_bag::bag_fbag(bool_(), sort_fbag::cons_(bool_(), true_(), number(sort_pos::pos(), "1"), fbag_empty_));

  // Using all operations
  BOOST_CHECK(print_check(bag_empty, "{:}"));
  BOOST_CHECK(print_check(sort_bag::bag_fbag(bool_(), sort_fbag::empty(bool_())), "{:}"));
  BOOST_CHECK(print_check(sort_bag::constructor(bool_(), sort_bag::zero_function(bool_()), fbag_empty_), "{:}"));
  BOOST_CHECK(print_check(sort_bag::in(bool_(), false_(), bag_empty), "false in {:}"));
  BOOST_CHECK(print_check(sort_bag::in(bool_(), false_(), bag_false), "false in {false: 1}"));
  BOOST_CHECK(print_check(sort_bag::count(bool_(), false_(), bag_true), "count(false, {true: 1})"));
  BOOST_CHECK(print_check(sort_bag::union_(bool_(), bag_false, bag_true), "{false: 1} + {true: 1}"));
  BOOST_CHECK(print_check(sort_bag::intersection(bool_(), bag_false, bag_true), "{false: 1} * {true: 1}"));
  BOOST_CHECK(print_check(sort_bag::difference(bool_(), bag_false, bag_true), "{false: 1} - {true: 1}"));

  // Some parsed expressions
  BOOST_CHECK(print_check(parse_data_expression("{true: 2}"), "{true: 2}"));
  BOOST_CHECK(print_check(parse_data_expression("{false: 3, true: 1}"), "{false: 3, true: 1}"));
  BOOST_CHECK(print_check(parse_data_expression("{ b: Bool | if(b, Pos2Nat(2), Pos2Nat(3)) }"), "{ b: Bool | if(b, 2, 3) }"));
  BOOST_CHECK(print_check(parse_data_expression("{ x: Nat | x * x }"), "{ x: Nat | x * x }"));
  BOOST_CHECK(print_check(parse_data_expression("{ x: Nat | (lambda y: Nat. y * y)(x) }"), "{ x: Nat | (lambda y: Nat. y * y)(x) }"));

  // Some types
  BOOST_CHECK(print_check(sort_fbag::fbag(bool_()), "FBag(Bool)"));
  BOOST_CHECK(print_check(parse_sort_expression("Bag(Nat)"), "Bag(Nat)"));
}

BOOST_AUTO_TEST_CASE(test_function_update_print)
{
  PRINT_CHECK(function_update(sort_nat::nat(), sort_bool::bool_()), "@func_update");
  PRINT_CHECK(parse_data_expression("(lambda x: Bool. x)[true -> false]"), "(lambda x: Bool. x)[true -> false]");
  PRINT_CHECK(parse_data_expression("(lambda x: Bool. x)[true -> false][false -> true]"), "(lambda x: Bool. x)[true -> false][false -> true]");
  PRINT_CHECK(parse_data_expression("(lambda n: Nat. n mod 2 == 0)[0 -> false]"), "(lambda n: Nat. n mod 2 == 0)[0 -> false]");
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_bool_print)
{
  BOOST_CHECK(print_container_check(sort_bool::bool_generate_equations_code()));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_pos_print)
{
  BOOST_CHECK(print_container_check(sort_pos::pos_generate_equations_code()));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_nat_print)
{
  BOOST_CHECK(print_container_check(sort_nat::nat_generate_equations_code()));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_int_print)
{
  BOOST_CHECK(print_container_check(sort_int::int_generate_equations_code()));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_real_print)
{
  BOOST_CHECK(print_container_check(sort_real::real_generate_equations_code()));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_fset_print)
{
  BOOST_CHECK(print_container_check(sort_fset::fset_generate_equations_code(sort_bool::bool_())));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_set_print)
{
  BOOST_CHECK(print_container_check(sort_set::set_generate_equations_code(sort_bool::bool_())));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_fbag_print)
{
  BOOST_CHECK(print_container_check(sort_fbag::fbag_generate_equations_code(sort_bool::bool_())));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_bag_print)
{
  BOOST_CHECK(print_container_check(sort_bag::bag_generate_equations_code(sort_bool::bool_())));
}

BOOST_AUTO_TEST_CASE(test_standard_sort_expressions)
{
  BOOST_CHECK(print_check(sort_bool::bool_(), "Bool"));
  BOOST_CHECK(print_check(sort_pos::pos(), "Pos"));
  BOOST_CHECK(print_check(sort_nat::nat(), "Nat"));
  BOOST_CHECK(print_check(sort_int::int_(), "Int"));
  BOOST_CHECK(print_check(sort_real::real_(), "Real"));
}

BOOST_AUTO_TEST_CASE(test_mod)
{
  data::data_expression x = parse_data_expression("(1 + 2) mod 3");
  BOOST_CHECK(sort_nat::is_mod_application(x));

  application left = atermpp::down_cast<application>(sort_nat::left(x));
  std::cout << "left = " << left << " " << data::pp(left) << std::endl;
  BOOST_CHECK(data::detail::is_plus(left));

  application left1 = atermpp::down_cast<application>(detail::remove_numeric_casts(left));
  std::cout << "left1 = " << left1 << " " << data::pp(left1) << std::endl;
  BOOST_CHECK(data::detail::is_plus(left1));

  BOOST_CHECK(data::pp(x) == "(1 + 2) mod 3");
  std::cout << "x = " << x << " " << data::pp(x) << std::endl;

  x = parse_data_expression("(2 - 1) mod 3");
  left = atermpp::down_cast<application>(sort_int::left(x));
  std::cout << "left = " << left << " " << data::pp(left) << std::endl;
  BOOST_CHECK(data::detail::is_minus(left));
  std::cout << "left_precedence(left) = " << left_precedence(left) << std::endl;

  BOOST_CHECK(data::sort_nat::is_nat(x.sort()));
  BOOST_CHECK(data::sort_int::is_mod_application(x));

  left1 = atermpp::down_cast<application>(detail::remove_numeric_casts(left));
  std::cout << "left1 = " << left1 << " " << data::pp(left1) << std::endl;
  BOOST_CHECK(data::detail::is_minus(left1));
  std::cout << "left_precedence(left1) = " << left_precedence(left1) << std::endl;

  BOOST_CHECK(data::pp(x) == "(2 - 1) mod 3");
  std::cout << "x = " << x << " " << data::pp(x) << std::endl;
}

BOOST_AUTO_TEST_CASE(test_sort_expressions)
{
  data::sort_expression x = parse_sort_expression("Bool # Nat -> (Pos -> Real)");
  std::string xtext = data::pp(x);
  data::sort_expression y = parse_sort_expression(xtext);
  std::string ytext = data::pp(y);
  std::cout << "original = " << "Bool # Nat -> (Pos -> Real)" << std::endl;
  std::cout << "xtext    = " << xtext << std::endl;
  std::cout << "ytext    = " << ytext << std::endl;
  BOOST_CHECK(x == y);
  BOOST_CHECK(xtext == ytext);
  BOOST_CHECK(data::pp(data::untyped_sort()) == "untyped_sort");
}

BOOST_AUTO_TEST_CASE(test_set_print2)
{
  using namespace sort_bool;

  data_expression one = parse_data_expression("1");
  data_expression x = sort_fset::insert(sort_nat::nat(), one, sort_fset::empty(sort_nat::nat()));
  sort_expression s = sort_nat::nat();

  data_expression true_  = sort_set::false_function(s);
  data_expression false_ = sort_set::true_function(s);
  data_expression f      = parse_function_symbol("f: Pos -> Bool");

  data_expression x1 = sort_set::constructor(s, true_, x);
  data_expression x2 = sort_set::constructor(s, false_, x);
  data_expression x3 = sort_set::constructor(s, f, x);

  BOOST_CHECK_EQUAL(data::pp(x1), "{1}");
  BOOST_CHECK_EQUAL(data::pp(x2), "!{1}");
  BOOST_CHECK_EQUAL(data::pp(x3), "{ x: Pos | f(x) != x in {1} }");
}

BOOST_AUTO_TEST_CASE(test_fset_print)
{
  using namespace sort_bool;

  data_expression one = parse_data_expression("1");
  data_expression x = parse_data_expression("{1, 2}");
  data_expression y = parse_data_expression("{3}");
  sort_expression s = sort_pos::pos();
  data_expression f = parse_function_symbol("f: Pos -> Bool");
  data_expression g = parse_function_symbol("g: Pos -> Bool");
  data_expression false_ = sort_set::false_function(s);
  data_expression true_ = sort_set::true_function(s);

  data_expression xy_union = sort_fset::fset_union(s, false_, false_, x, y);
  data_expression xy_intersection = sort_fset::fset_intersection(s, false_, false_, x, y);
  data_expression xy_difference = sort_fset::difference(s, x, y);
  data_expression xy_in = sort_fset::in(s, one, x);

  BOOST_CHECK_EQUAL(data::pp(xy_union)       , "{1, 2} + {3}");
  BOOST_CHECK_EQUAL(data::pp(xy_intersection), "{1, 2} * {3}");
  BOOST_CHECK_EQUAL(data::pp(xy_difference)  , "{1, 2} - {3}");
  BOOST_CHECK_EQUAL(data::pp(xy_in)          , "1 in {1, 2}");

  xy_union = sort_fset::fset_union(s, f, false_, x, y);
  xy_intersection = sort_fset::fset_intersection(s, f, false_, x, y);
  BOOST_CHECK_EQUAL(data::pp(xy_union)       , "{1, 2} + { x: Pos | !f(x) && x in {3} }");
  BOOST_CHECK_EQUAL(data::pp(xy_intersection), "{1, 2} * { x: Pos | !f(x) && x in {3} }");

  xy_union = sort_fset::fset_union(s, f, g, x, y);
  xy_intersection = sort_fset::fset_intersection(s, f, g, x, y);
  BOOST_CHECK_EQUAL(data::pp(xy_union)       , "{ x: Pos | !g(x) && x in {1, 2} } + { x: Pos | !f(x) && x in {3} }");
  BOOST_CHECK_EQUAL(data::pp(xy_intersection), "{ x: Pos | !g(x) && x in {1, 2} } * { x: Pos | !f(x) && x in {3} }");
}

BOOST_AUTO_TEST_CASE(test_precedence)
{
  data::data_expression x = parse_data_expression("exists b:Bool. true");
  BOOST_CHECK(is_exists(x));
  BOOST_CHECK(left_precedence(x) == 1);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
