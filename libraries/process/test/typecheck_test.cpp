// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file typecheck_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/process/parse.h"
#include "mcrl2/process/typecheck.h"

using namespace mcrl2;

void test_typechecker_case(std::string const& spec, bool const expected_result)
{
  std::clog << std::endl
            << "<---- testing specification: ---->" << std::endl
            << spec << std::endl;
  if (expected_result)
  {
    std::clog << "expected result: success" << std::endl;
    try
    {
      process::parse_process_specification(spec);
    }
    catch (mcrl2::runtime_error& e) // Catch errors and print them, such that all cases are treated.
    {
      std::clog << "type checking failed with error: " << std::endl
                << e.what() << std::endl;
      BOOST_CHECK(false);
    }
  }
  else
  {
    std::clog << "expected result: failure" << std::endl;
    BOOST_CHECK_THROW(process::parse_process_specification(spec), mcrl2::runtime_error);
  }
}

BOOST_AUTO_TEST_CASE(test_concat_element)
{
  // Example provided by Tim Willemse, 7 Oct 2009
  test_typechecker_case(
    "map place : List(Nat) -> List(Nat); \n"
    "                                    \n"
    "var l : List(Nat);                  \n"
    "                                    \n"
    "eqn place (l) = head(l) ++ tail(l); \n"
    "                                    \n"
    "init delta;                         \n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_concat_lists)
{
  test_typechecker_case(
    "map place : List(Nat) -> List(Nat); \n"
    "                                    \n"
    "var l : List(Nat);                  \n"
    "                                    \n"
    "eqn place (l) = l ++ tail(l);       \n"
    "                                    \n"
    "init delta;                         \n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_bug_528a)
{
  test_typechecker_case(
    "sort S = struct c;                  \n"
    "map succ_: S -> S;                   \n"
    "eqn succ_(c) = c;                    \n"
    "init delta;                         \n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_bug_528b)
{
  test_typechecker_case(
    "sort S,T;                           \n"
    "map  count_: S # T -> Nat;           \n"
    "var  x:S;                           \n"
    "     y:T;                           \n"
    "eqn  count_(x, y) = 0;               \n"
    "init delta;                         \n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_bug_528c)
{
  test_typechecker_case(
    "sort S;                             \n"
    "map  count_: S -> Nat;               \n"
    "var  x:S;                           \n"
    "eqn  count_(x) = 0;                  \n"
    "init delta;                         \n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_bug_528d)
{
  test_typechecker_case(
    "map  count: Pos#Bag(Pos)->Nat;      \n"
    "     f:Nat->Pos;                    \n"
    "                                    \n"
    "act  a:Nat;                         \n"
    "                                    \n"
    "proc P1(i: Nat) = a(i);             \n"
    "                                    \n"
    "init P1(count(3,{3:4}));            \n",
    false
  );
}


BOOST_AUTO_TEST_CASE(test_bug_663a)
{
  test_typechecker_case(
    "map const: Pos;                  \n"
    "eqn const = 10;                  \n"
    "                                 \n"
    "proc P1(i: Nat) = delta;         \n"
    "                                 \n"
    "init P1(const);                  \n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_bug_663b)
{
  test_typechecker_case(
    "map const: Pos;                  \n"
    "eqn const = 10;                  \n"
    "                                 \n"
    "proc P1(i: Nat) = delta;         \n"
    "                                 \n"
    "init P1(Nat2Pos(Pos2Nat(const)));\n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_bug_663c)
{
  test_typechecker_case(
    "map const: Pos;                  \n"
    "eqn const = 10;                  \n"
    "                                 \n"
    "proc P1(i: Nat) = delta;         \n"
    "                                 \n"
    "init P1(Pos2Nat(const));\n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_bug_644)
{
  test_typechecker_case(
    "cons maybe: Bool;                \n"
    "init delta;                      \n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_bug_626a)
{
  test_typechecker_case(
    "sort S = struct c( x: Int );     \n"
    "                                 \n"
    "map f :(S -> Bool)#S -> S;       \n"
    "var pred: S -> Bool;             \n"
    "    s: S;                        \n"
    "eqn f (pred, s) = s;             \n"
    "                                 \n"
    "act a: (S);                      \n"
    "                                 \n"
    "init a( f( lambda x:S. x(x) < 0 , c( 0 ) ) );\n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_bug_626b)
{
  test_typechecker_case(
    "sort S = struct c( x: Int );     \n"
    "                                 \n"
    "map f :(S -> Bool)#S -> S;       \n"
    "var pred: S -> Bool;             \n"
    "    s: S;                        \n"
    "eqn f (pred, s) = s;             \n"
    "                                 \n"
    "act a: (S);                      \n"
    "                                 \n"
    "init a( f( lambda i:S. x(i) < 0 , c( 0 ) ) );\n",
    true
  );
}

// First test case for issue #629, constructor domain is empty.
BOOST_AUTO_TEST_CASE(test_bug_629a)
{
  test_typechecker_case(
    "sort L_1=struct insert(L_1) ;    \n"
    "init delta;                      \n",
    false
  );
}

// Second test case for issue #629, constructor domain is empty.
BOOST_AUTO_TEST_CASE(test_bug_629b)
{
  test_typechecker_case(
    "sort L_2=struct insert(L_3);     \n"
    "     L_3=struct insert(L_2);     \n"
    "init delta;                      \n",
    false
  );
}

// Third test case for issue #629, constructor domain is empty.
BOOST_AUTO_TEST_CASE(test_bug_629c)
{
  test_typechecker_case(
    "sort D;                          \n"
    "cons f:D->D;                     \n"
    "init delta;                      \n",
    false
  );
}

// fourth test case for issue #629, constructor domain is empty.
BOOST_AUTO_TEST_CASE(test_bug_629d)
{
  test_typechecker_case(
    "sort D =struct f(struct g(D));"
    "init delta;",
    false
  );
}

// fifth test case for issue #629, constructor domain is NOT empty.
BOOST_AUTO_TEST_CASE(test_bug_629e)
{
  test_typechecker_case(
    "sort D =struct f(struct g(Nat));"
    "init delta;",
    true
  );
}

// Tricky test case that should succeed, as sorts are not recursively defined through sort containers.
BOOST_AUTO_TEST_CASE(test_recursive_a)
{
  test_typechecker_case(
    "sort MyRecType=struct f | g(MyRecType);"
    "     D=List(MyRecType);"
    "init delta;",
    true
  );
}


// Tricky test case that should succeed, as recursive sorts are are defined through the list containers
// are allowed.
BOOST_AUTO_TEST_CASE(test_recursive_b)
{
  test_typechecker_case(
    "sort MyRecType=struct f | g(List(MyRecType));"
    "init delta;",
    true
  );
}

// Tricky test case that should succeed, as sorts are not recursively defined through sort containers.
BOOST_AUTO_TEST_CASE(test_recursive_c)
{
  test_typechecker_case(
    "sort MyRecType=struct f | g(MyRecType);"
    "     D=List(MyRecType)->MyRecType;"
    "init delta;",
    true
  );
}

// Tricky test case that should fail, as recursive sorts are are defined through sort containers
// but in this case it is not that easy to see.
BOOST_AUTO_TEST_CASE(test_recursive_d)
{
  test_typechecker_case(
    "sort MyRecType=struct f | g(MyRecType->Nat);"
    "init delta;",
    false
  );
}

// Test case below went wrong, because sort expression was confused with a function symbol
BOOST_AUTO_TEST_CASE(test_sort_expression_vs_function_symbol)
{
  test_typechecker_case(
    "map  const: Pos;                 \n"
    "     f:Nat->Pos;                 \n"
    "eqn  const  =  10;               \n"
    "proc P1(i: Nat) = delta;         \n"
    "init P1(f(const));               \n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_real_zero)
{
  test_typechecker_case(
    "sort T = Real;\n"
    "map  x: List(T) -> List(T);\n"
    "var  l: List(T);\n"
    "     r: T;\n"
    "eqn  x(r |> l) = (r+0) |> l;\n"
    "act  a: List(T);\n"
    "init a(x([0]));\n",
    true
  );
}

// The following example tests whether a double assignment in a
// process is properly caught by the typechecker.
BOOST_AUTO_TEST_CASE(test_double_variable_assignment_in_process)
{
  test_typechecker_case(
    "proc X( v :Bool  ) = tau.  X( v = true, v = false );\n"
    "init X(true);",
    false);
}

BOOST_AUTO_TEST_CASE(test_typecheck)
{
  std::string text =
    "act  a;\n"
    "glob d,c,b: Bool;\n"
         "n,m: Nat;\n"
         "p: Pos;\n"
    "proc P(b,c: Bool) = a . P(c = true);\n"
    "init delta;\n"
    ;
  process::process_specification procspec = process::parse_process_specification_new(text);
  process::type_check(procspec);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
