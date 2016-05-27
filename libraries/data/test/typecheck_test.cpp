// Author(s): Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file typecheck_test.cpp
/// \brief Add your file description here.

// Some tests rely on type check failures, so we have to set this flag.
#define MCRL2_DISABLE_TYPECHECK_ASSERTIONS

#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/data/parse.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/untyped_sort.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/experimental/type_check_tree.h"
#include "mcrl2/utilities/text_utility.h"

using namespace mcrl2;

inline
data::sort_expression pos()
{
  return data::sort_pos::pos();
}

inline
data::sort_expression nat()
{
  return data::sort_nat::nat();
}

inline
data::sort_expression list(const data::sort_expression& x)
{
  return data::sort_list::list(x);
}

inline
data::variable var(const std::string& name, const data::sort_expression& sort)
{
  return data::variable(name, sort);
}

// Expected failures, these are not going to be fixed in the current
// implementation of the type checker
//BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(test_multiple_variables, 1)
//BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(test_multiple_variables_reversed, 1)
//BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(test_matching_ambiguous, 1)               // Fails because of reordering in type checker / pretty printer
//BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(test_matching_ambiguous_rhs, 1)           // Fails because of reordering in type checker / pretty printer
//BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(test_ambiguous_function_application4, 1)  // Fails because of reordering in type checker / pretty printer
//BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(test_ambiguous_function_application4a, 1) // Fails because of reordering in type checker / pretty printer

// Parse functions that do not change any context (i.e. do not typecheck and
// normalise sorts).
data::sort_expression parse_sort_expression(const std::string& de_in)
{
  data::sort_expression result;
  try {
    result = data::detail::parse_sort_expression_new(de_in);
    std::string de_out = data::pp(result);
    if (de_in != de_out)
    {
      std::clog << "aterm : " << result << std::endl;
      std::clog << "de_in : " << de_in << std::endl;
      std::clog << "de_out: " << de_out << std::endl;
      std::clog << "The following sort expressions should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
      BOOST_CHECK_EQUAL(de_in, de_out);
    }
  }
  catch (...)
  {
    BOOST_CHECK(false);
  }
  return result;
}

data::data_expression parse_data_expression(const std::string& de_in)
{
  data::data_expression result;
  try {
    result = data::detail::parse_data_expression_new(de_in);
#ifdef MCRL2_ENABLE_TYPECHECK_PP_TESTS
    std::string de_out = data::pp(result);
    if (de_in != de_out)
    {
      std::clog << "aterm : " << result << std::endl;
      std::clog << "de_in : " << de_in << std::endl;
      std::clog << "de_out: " << de_out << std::endl;
      std::clog << "The following data expressions should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
      BOOST_CHECK_EQUAL(de_in, de_out);
    }
#endif
  }
  catch (...)
  {
    BOOST_CHECK(false);
  }
  return result;
}

data::data_specification parse_data_specification(const std::string& de_in, bool expect_success = true)
{
  data::data_specification result;
  try {
    result = data::detail::parse_data_specification_new(de_in);
    std::string de_out = data::pp(result);

    std::string input = utilities::trim_copy(de_in);
    std::string output = utilities::trim_copy(de_out);
    if (input != output)
    {
      std::clog << "aterm : " << data::detail::data_specification_to_aterm_data_spec(result) << std::endl;
      std::clog << "de_in : " << de_in << std::endl;
      std::clog << "de_out: " << de_out << std::endl;
      std::clog << "The following data specifications should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
      BOOST_CHECK_EQUAL(input, output);
    }
  }
  catch (...)
  {
    BOOST_CHECK(!expect_success);
  }
  return result;
}

void test_data_expression(const std::string& de_in,
                          const data::variable_vector& variable_context,
                          bool expect_success = true,
                          const std::string& expected_sort = "",
                          bool test_type_checker = true)
{
  data::data_expression x(parse_data_expression(de_in));
  std::clog << std::endl
            << "==========================================" << std::endl
            << "Testing type checking of data expression: " << std::endl
            << "  de_in:  " << de_in << std::endl
            << "  de_out: " << pp(x) << std::endl
            << "  expect success: " << (expect_success?("yes"):("no")) << std::endl
            << "  expected type: " << expected_sort << std::endl
            << "  detected type: " << pp(x.sort()) << " (before typecheckeing) " << std::endl;


  if (test_type_checker)
  {
    if (expect_success)
    {
      BOOST_CHECK_NO_THROW(x = data::type_check_data_expression(x, variable_context));
      BOOST_CHECK_NE(x, data::data_expression());

      std::string de_out = data::pp(x);
      //std::clog << "The following data expressions should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
//#ifdef MCRL2_ENABLE_TYPECHECK_PP_TESTS
      BOOST_CHECK_EQUAL(de_in, de_out);
//#endif
      // TODO: this check should be uncommented
      //BOOST_CHECK(!search_sort_expression(x.sort(), data::untyped_sort()));
      if (expected_sort != "")
      {
        BOOST_CHECK_EQUAL(x.sort(), parse_sort_expression(expected_sort));
        std::clog << "    expression x in internal format: " << x << std::endl;
      }
      else
      {
        std::clog << "  failed to typecheck" << std::endl;
      }
    }
    else
    {
      BOOST_CHECK_THROW(x = data::type_check_data_expression(x), mcrl2::runtime_error);
    }
  }
}

void test_data_expression(const std::string& de_in,
                          bool expect_success = true,
                          const std::string& expected_sort = "",
                          bool test_type_checker = true)
{
  data::variable_vector v;
  test_data_expression(de_in, v, expect_success, expected_sort, test_type_checker);
}

BOOST_AUTO_TEST_CASE(test_true)
{
  //test boolean data expressions
  test_data_expression("true", true, "Bool");
}

BOOST_AUTO_TEST_CASE(test_if)
{
  test_data_expression("if(true, true, false)", true, "Bool");
}

BOOST_AUTO_TEST_CASE(test_not)
{
  test_data_expression("!true", true, "Bool");
}

BOOST_AUTO_TEST_CASE(test_and)
{
  test_data_expression("true && false", true, "Bool");
}

BOOST_AUTO_TEST_CASE(test_zero)
{
  test_data_expression("0", true, "Nat");
}

BOOST_AUTO_TEST_CASE(test_minus_one)
{
  test_data_expression("-1", true, "Int");
}

BOOST_AUTO_TEST_CASE(test_zero_plus_one)
{
  test_data_expression("0 + 1", true, "Pos");
}

BOOST_AUTO_TEST_CASE(test_one_plus_zero)
{
  test_data_expression("1 + 0", true, "Pos");
}

BOOST_AUTO_TEST_CASE(test_zero_plus_zero)
{
  test_data_expression("0 + 0", true, "Nat");
}

BOOST_AUTO_TEST_CASE(test_one_plus_one)
{
  test_data_expression("1 + 1", true, "Pos");
}

BOOST_AUTO_TEST_CASE(test_one_times_two_plus_three)
{
  test_data_expression("1 * 2 + 3", true, "Pos");
}

BOOST_AUTO_TEST_CASE(test_empty_list)
{
  test_data_expression("[]", true); // List unknown
}

BOOST_AUTO_TEST_CASE(test_empty_list_concat)
{
  test_data_expression("[] ++ []", true); // List unknown
}

BOOST_AUTO_TEST_CASE(test_empty_list_size)
{
  test_data_expression("#[]", true, "Nat");
}

BOOST_AUTO_TEST_CASE(test_empty_list_in)
{
  test_data_expression("true in []", true, "Bool");
}

BOOST_AUTO_TEST_CASE(test_list_true_false)
{
  test_data_expression("[true, false]", true, "List(Bool)");
}

BOOST_AUTO_TEST_CASE(test_list_zero)
{
  test_data_expression("[0]", true, "List(Nat)");
}

BOOST_AUTO_TEST_CASE(test_list_one_two)
{
  test_data_expression("[1, 2]", true, "List(Pos)");
}

BOOST_AUTO_TEST_CASE(test_list_zero_concat_one_two)
{
  test_data_expression("[0] ++ [1, 2]", true, "List(Nat)");
}

BOOST_AUTO_TEST_CASE(test_list_nat_pos)
{
  test_data_expression("[0, 1, 2]", true, "List(Nat)");
}

BOOST_AUTO_TEST_CASE(test_list_pos_nat)
{
  test_data_expression("[1, 0, 2]", true, "List(Nat)");
}

BOOST_AUTO_TEST_CASE(test_list_nat_concat_one_two)
{
  test_data_expression("l ++ [1, 2]", { var("l", list(nat())) }, true, "List(Nat)");
}

BOOST_AUTO_TEST_CASE(test_list_pos_concat_one_two)
{

  test_data_expression("l ++ [1, 2]", { var("l", list(pos())) }, true, "List(Pos)");
}

BOOST_AUTO_TEST_CASE(test_list_zero_concat_list_pos)
{
  test_data_expression("[0] ++ l", { var("l", list(pos())) }, false);
}

BOOST_AUTO_TEST_CASE(test_list_zero_concat_list_nat)
{
  test_data_expression("[0] ++ l", { var("l", list(nat())) }, true, "List(Nat)");
}

BOOST_AUTO_TEST_CASE(test_list_pos_concat_list_nat)
{
  test_data_expression("x ++ y", { var("x", list(pos())), var("y", list(nat())) }, false);
}

BOOST_AUTO_TEST_CASE(test_list_is_list_nat)
{
  test_data_expression("x == y", { var("x", list(pos())),  var("y", list(nat())) }, false);
}

BOOST_AUTO_TEST_CASE(test_emptyset)
{
  test_data_expression("{}", true);
}

BOOST_AUTO_TEST_CASE(test_emptyset_complement)
{
  test_data_expression("!{}", true);
}

BOOST_AUTO_TEST_CASE(test_emptyset_complement_subset)
{
  test_data_expression("!{} <= {}", true);
}

BOOST_AUTO_TEST_CASE(test_emptyset_complement_subset_reverse)
{
  test_data_expression("{} <= !{}", true);
}

BOOST_AUTO_TEST_CASE(test_set_true_false)
{
  test_data_expression("{ true, false }", true, "FSet(Bool)");
}

BOOST_AUTO_TEST_CASE(test_set_numbers)
{
  test_data_expression("{ 1, 2, -7 }", true, "FSet(Int)");
}

BOOST_AUTO_TEST_CASE(test_set_comprehension)
{
  test_data_expression("{ x: Nat | x mod 2 == 0 }", true, "Set(Nat)");
}

BOOST_AUTO_TEST_CASE(test_emptybag)
{
  test_data_expression("{:}", true);
}

BOOST_AUTO_TEST_CASE(test_emptybag_complement)
{
  test_data_expression("!{:}", false);
}
BOOST_AUTO_TEST_CASE(test_bag_true_false)
{
  test_data_expression("{ true: 1, false: 2 }", true, "FBag(Bool)");
}

BOOST_AUTO_TEST_CASE(test_bag_numbers)
{
  test_data_expression("{ 1: 1, 2: 2, -8: 8 }", true, "FBag(Int)");
}

BOOST_AUTO_TEST_CASE(test_bag_comprehension)
{
  test_data_expression("{ x: Nat | (lambda y: Nat. y * y)(x) }", true, "Bag(Nat)");
}

BOOST_AUTO_TEST_CASE(test_function_updates)
{
  //test function updates
  test_data_expression("(lambda x: Bool. x)[true -> false]", true);
  test_data_expression("(lambda x: Bool. x)[0 -> false]", false);
  test_data_expression("(lambda x: Bool. x)[true -> false][false -> true]", true);
  test_data_expression("(lambda n: Nat. n mod 2 == 0)[0 -> false]", true);
}

BOOST_AUTO_TEST_CASE(test_inline_struct_recogniser)
{
  test_data_expression("lambda x: struct t?is_t. x == t", false);
}

BOOST_AUTO_TEST_CASE(test_inline_struct)
{
  test_data_expression("lambda x: struct t. x == t", false);
}

BOOST_AUTO_TEST_CASE(test_inline_structs_compare)
{
  test_data_expression("lambda x,y: struct t. x == y", true, "struct t # struct t -> Bool");
}

BOOST_AUTO_TEST_CASE(test_inline_structs_compare_recogniser)
{
  test_data_expression("lambda x: struct t?is_t, y: struct t. x == y", false);
}

BOOST_AUTO_TEST_CASE(test_lambda_aliasing)
{
  test_data_expression("lambda f: Nat. lambda f: Nat -> Bool. f(f)", false);
}

BOOST_AUTO_TEST_CASE(test_forall_structs_compare)
{
  test_data_expression("forall x,y: struct t. x == y", true, "Bool");
}

BOOST_AUTO_TEST_CASE(test_forall_simple)
{
  test_data_expression("forall n: Nat. n >= 0", true, "Bool");
}

BOOST_AUTO_TEST_CASE(test_forall_simple_nat_vs_int)
{
  test_data_expression("forall n: Nat. n > -1", true, "Bool");
}

BOOST_AUTO_TEST_CASE(test_exists_structs_compare)
{
  test_data_expression("exists x,y: struct t. x == y", true, "Bool");
}

BOOST_AUTO_TEST_CASE(test_exists_simple)
{
  test_data_expression("exists n: Nat. n > 481", true, "Bool");
}

BOOST_AUTO_TEST_CASE(test_equal_context)
{
  test_data_expression("x == y", { var("x", parse_sort_expression("struct t?is_t")), var("y", parse_sort_expression("struct t?is_t")) }, true, "Bool");
}

BOOST_AUTO_TEST_CASE(test_not_equal_context)
{
  test_data_expression("x == y", { var("x", parse_sort_expression("struct t")), var("y", parse_sort_expression("struct t?is_t")) }, false);
}

BOOST_AUTO_TEST_CASE(test_where)
{
  test_data_expression("x + y whr x = 3, y = 10 end", true, "Pos");
}

BOOST_AUTO_TEST_CASE(test_where_var_one_occurs_in_two)
{
  test_data_expression("x + y whr x = 3, y = x + 10 end", false);
}

BOOST_AUTO_TEST_CASE(test_where_var_one_and_two_occur_in_two)
{
  test_data_expression("x + y whr x = 3, y = x + y + 10 end", false);
}

BOOST_AUTO_TEST_CASE(test_where_var_two_occurs_in_one)
{
  test_data_expression("x + y whr x = y + 10, y = 3 end", false);
}

BOOST_AUTO_TEST_CASE(test_where_var_one_occurs_in_two_and_vice_versa)
{
  test_data_expression("x + y whr x = y + 10, y = x + 3 end", false);
}

BOOST_AUTO_TEST_CASE(test_where_in_context)
{
  test_data_expression("x + y whr x = 3, y = 10 end", { var("x", pos()), var("y", nat()) }, true, "Pos");
}

BOOST_AUTO_TEST_CASE(test_where_var_one_occurs_in_two_in_context)
{
  test_data_expression("x + y whr x = 3, y = x + 10 end", { var("x", pos()), var("y", nat()) }, true, "Pos");
}

BOOST_AUTO_TEST_CASE(test_where_var_one_and_two_occur_in_two_in_context)
{
  test_data_expression("x + y whr x = 3, y = x + y + 10 end", { var("x", pos()), var("y", nat()) }, true, "Pos");
}

BOOST_AUTO_TEST_CASE(test_where_var_two_occurs_in_one_in_context)
{
  test_data_expression("x + y whr x = y + 10, y = 3 end", { var("x", pos()), var("y", nat()) }, true, "Pos");
}

BOOST_AUTO_TEST_CASE(test_where_var_one_occurs_in_two_and_vice_versa_in_context)
{
  test_data_expression("x + y whr x = y + 10, y = x + 3 end", { var("x", pos()), var("y", nat()) }, true, "Pos");
}

BOOST_AUTO_TEST_CASE(test_where_mix_nat_pos_list)
{
  test_data_expression("x ++ y whr x = [0, y], y = [x] end", { var("x", pos()), var("y", nat()) }, false);
}

BOOST_AUTO_TEST_CASE(test_where_mix_nat_list)
{
  test_data_expression("x ++ y whr x = [0, y], y = [x] end", { var("x", nat()), var("y", nat()) }, true, "List(Nat)");
}

BOOST_AUTO_TEST_CASE(test_upcast_pos2nat)
{
  test_data_expression("x + y", { var("x", pos()), var("y", nat()) }, true, "Pos");
  test_data_expression("x == y", { var("x", pos()), var("y", nat()) }, true, "Bool");
}

void test_data_specification(const std::string& ds_in,
                             bool expect_success = true,
                             bool test_type_checker = true)
{
  data::data_specification ds(parse_data_specification(ds_in, expect_success));

  if (test_type_checker)
  {

    if (expect_success)
    {
      data::type_check_data_specification(ds);
      //Cannot pretty print a data specification anymore.
      //std::string ds_out = data::pp(ds);
      //BOOST_CHECK_EQUAL(ds_in, ds_out);
    }
    else
    {
      BOOST_CHECK_THROW(data::type_check_data_specification(ds), mcrl2::runtime_error);
    }
  }
}

BOOST_AUTO_TEST_CASE(test_data_specification_struct_with_projection)
{
  //test data specification involving structured sorts
  //in which projection functions are reused
  test_data_specification(
    "sort S = struct c(p: Bool) | d(p: Bool, q: S);\n"
  );
}

// What is the desired result here? I would expect an exception (JK)
BOOST_AUTO_TEST_CASE(test_duplicate_sort)
{
  test_data_specification(
    "sort S = struct c;\n"
    "     S = Nat;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_data_specification_constructor_same_signature)
{
  //test data specification involving multiple constructors/functions with
  //different signatures
  test_data_specification(
    "sort S;\n"
    "     T;\n"
    "\n"
    "cons f: S;\n"
    "     f: T;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_data_specification_constructor_map_same_signature)
{
  //test data specification involving multiple constructors/functions with
  //different signatures
  test_data_specification(
    "sort S;\n"
    "     T;\n"
    "\n"
    "cons f: S;\n"
    "\n"
    "map  f: T;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_data_specification_constructor_different_signature)
{
  test_data_specification(
    "sort S;\n"
    "     T;\n"
    "\n"
    "cons f: S;\n"
    "     f: S -> T;\n"
  );
}

BOOST_AUTO_TEST_CASE(test_data_specification_nested_struct)
{
  test_data_specification(
    "sort S = struct t(struct e(Nat));\n"
  );
}

//BOOST_AUTO_TEST_CASE(test_multiple_variables)
//{
//  test_data_specification(
//    "sort S;\n\n"
//    "var  x: Nat;\n"
//    "     x: S;\n"
//    "eqn  x == x + 1  =  true;\n",
//    false
//  );
//}
//
//BOOST_AUTO_TEST_CASE(test_multiple_variables_reversed)
//{
//  test_data_specification(
//    "sort S;\n\n"
//    "var  x: S;\n"
//    "     x: Nat;\n"
//    "eqn  x == x + 1  =  true;\n",
//    false
//  );
//}

BOOST_AUTO_TEST_CASE(test_sort_as_variable)
{
  test_data_specification(
    "sort S;\n\n"
    "map  S: S -> Bool;\n\n"
    "var  S: S;\n"
    "eqn  S(S)  =  S == S;\n"
  );
}

/* BOOST_AUTO_TEST_CASE(test_predefined_aliases)   // This test case leads to a parse error, not a typecheck error.
{
  test_data_specification(
    "sort Nat = Int;\n",
    false, // parse error
    false  // so do not test type checker
  );
} */

/* BOOST_AUTO_TEST_CASE(test_conflicting_aliases) // This test case leads to a parse error, due to the use of Nat.
                                               // This is not a typecheck error. Therefore this case is outcommented.
{
  test_data_specification(
    "sort S = Nat;\n"
    "     S = T;\n"
    "     T = Int;\n",
    false
  );
} */

/* BOOST_AUTO_TEST_CASE(test_conflicting_aliases_predefined_left)  // This test case leads to a parse error, due to the use of Nat.
{
  test_data_specification(
    "sort Nat = S;\n"
    "     S = T;\n"
    "     T = Int;\n",
    false, // parse error
    false  // so do not test type checker
  );
} */

BOOST_AUTO_TEST_CASE(test_cyclic_aliases)
{
  test_data_specification(
    "sort S = U;\n"
    "     U = S;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_cyclic_aliases_indirect)
{
  test_data_specification(
    "sort S = U;\n"
    "     U = T;\n"
    "     T = S;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_matching)
{
  test_data_specification(
    "map  f: Pos # Nat -> Bool;\n\n"
    "var  x: Pos;\n"
    "     y: Nat;\n"
    "eqn  f(x, y)  =  true;\n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_matching_non_strict)
{
  test_data_specification(
    "map  f: Pos # Nat -> Bool;\n\n"
    "var  x: Pos;\n"
    "     y: Nat;\n"
    "eqn  f(x, x)  =  true;\n",
    true
  );
}

//BOOST_AUTO_TEST_CASE(test_matching_ambiguous)
//{
//  test_data_specification(
//    "map  f: Pos # Nat -> Bool;\n"
//    "     f: Nat # Nat -> Bool;\n\n"
//    "var  x: Pos;\n"
//    "     y: Nat;\n"
//    "eqn  f(x, y)  =  false;\n\n"
//    "var  x: Pos;\n"
//    "     y: Nat;\n"
//    "eqn  f(y, y)  =  true;\n",
//    true
//  );
//}
//
//BOOST_AUTO_TEST_CASE(test_matching_ambiguous_rhs)
//{
//  test_data_specification(
//    "map  f: Int;\n\n"
//    "var  x: Pos;\n"
//    "eqn  f(x)  =  -5;\n\n"
//    "var  x: Pos;\n"
//    "eqn  f(x)  =  3;\n",
//    false
//  );
//}

BOOST_AUTO_TEST_CASE(test_function_alias)
{
  test_data_specification(
    "sort Array = Nat -> Nat;\n\n"
    "map  update: Nat # Nat # Array -> Array;\n\n"
    "var  i,n: Nat;\n"
    "     f: Array;\n"
    "eqn  update(i, n, f)  =  lambda j: Nat. if(i == j, n, f(j));\n",
    true
  );
}

// Test case for bug #787
BOOST_AUTO_TEST_CASE(test_eqn_set_where)
{
  test_data_specification(
    "map  f_dot: Set(Bool);\n\n"
    "eqn  f_dot  =  if(true, {}, { o: Bool | true whr z=true end });\n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_recursive_function_sort)
{
  test_data_specification(
    "sort G;\n"
    "     F = F -> G;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_recursive_function_sort_reverse)
{
  test_data_specification(
    "sort G;\n"
    "     F = G -> F;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_recursive_struct_no_base)
{
  test_data_specification(
    "sort D = struct f(D);\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_recursive_struct_via_function)
{
  test_data_specification(
    "sort G = struct f(Nat -> G);\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_recursive_struct_list)
{
  test_data_specification(
    "sort P = struct b(x: List(P));\n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_recursive_struct_list_indirect)
{
  test_data_specification(
    "sort LP = List(P);\n"
    "     P = struct b(x: LP);\n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_alias_loop) // This is a correct declaration, Typical elements of sort B are [], [f([])].
{
  test_data_specification(
    "sort B = List(struct f(B));\n",
    true
  );
}

BOOST_AUTO_TEST_CASE(test_alias_loop_extended)
{
  test_data_specification(
    "sort B = List(struct f(B) | c);\n",
    true
  );
}


void test_data_expression_in_specification_context(const std::string& de_in,
    const std::string& ds_in,
    const data::variable_vector& variable_context,
    bool expect_success = true,
    const std::string& expected_sort = "",
    bool test_type_checker = true)
{
  data::data_specification ds(parse_data_specification(ds_in));

  if (test_type_checker)
  {
    data::type_check_data_specification(ds);
    std::string ds_out = data::pp(ds);
    if (utilities::trim_copy(ds_in) != utilities::trim_copy(ds_out))
    {
      std::clog << "Warning, ds_in != ds_out; [" << utilities::trim_copy(ds_in) << " != " << utilities::trim_copy(ds_out) << "]" << std::endl;
    }
    BOOST_CHECK_EQUAL(utilities::trim_copy(ds_in), utilities::trim_copy(ds_out));
  }

  data::data_expression de(parse_data_expression(de_in));

  if (test_type_checker)
  {
    if (expect_success)
    {
      de = data::type_check_data_expression(de, variable_context, ds);

      std::string de_out = data::pp(de);

      BOOST_CHECK_EQUAL(de_in, de_out);
      if (expected_sort != "")
      {
        BOOST_CHECK_EQUAL(de.sort(), parse_sort_expression(expected_sort));
      }
    }
    else
    {
      BOOST_CHECK_THROW(de = data::type_check_data_expression(de, variable_context, ds), mcrl2::runtime_error);
    }
  }
}

void test_data_expression_in_specification_context(const std::string& de_in,
    const std::string& ds_in,
    bool expect_success = true,
    const std::string& expected_sort = "",
    bool test_type_checker = true)
{
  data::variable_vector v;
  test_data_expression_in_specification_context(de_in, ds_in, v, expect_success, expected_sort, test_type_checker);
}

BOOST_AUTO_TEST_CASE(test_data_expressions_different_signature)
{
  test_data_expression_in_specification_context(
    "f(f)",
    "sort S;\n"
    "     T;\n"
    "\n"
    "cons f: S;\n"
    "     f: S -> T;\n",
    true,
    "T"
  );
}

BOOST_AUTO_TEST_CASE(test_data_expressions_struct)
{
  test_data_expression_in_specification_context(
    "x == t(e(3))",
    "sort S = struct t(struct e(Nat));\n",
    { var("x", data::basic_sort("S")) },
    true,
    "Bool"
  );
}

BOOST_AUTO_TEST_CASE(test_lambda_variable_aliasing)
{
  test_data_expression_in_specification_context(
    "lambda x: S. x(x)",
    "sort S;\n"
    "     T;\n",
    { var("x", make_function_sort(data::basic_sort("S"), data::basic_sort("T"))) },
    false);
}

BOOST_AUTO_TEST_CASE(test_lambda_anonymous_struct)
{
  test_data_expression_in_specification_context(
    "lambda x: struct t. f(x)",
    "map  f: struct t -> Bool;\n",
    true,
    "struct t -> Bool"
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_application)
{
  test_data_expression_in_specification_context(
    "f(f)",
    "map  f: Nat -> Bool;\n"
    "     f: Nat;\n",
    true,
    "Bool"
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_different_arity)
{
  test_data_expression_in_specification_context(
    "f",
    "map  f: Nat -> Bool;\n"
    "     f: Nat;\n",
    true,
    "Nat"
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_different_arity_reverse)
{
  test_data_expression_in_specification_context(
    "f",
    "map  f: Nat;\n"
    "     f: Nat -> Bool;\n",
    true,
    "Nat"
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_different_arity_larger)
{
  test_data_expression_in_specification_context(
    "f",
    "map  f: Nat -> Bool;\n"
    "     f: Nat # Nat -> Bool;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_different_arity_horrible)
{
  test_data_expression_in_specification_context(
    "f",
    "map  f: Nat -> Bool;\n"
    "     f: Nat # Nat -> Bool;\n"
    "     f: Nat;\n",
    true,
    "Nat"
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_different_arity_horrible_app1)
{
  test_data_expression_in_specification_context(
    "f(f)",
    "map  f: Nat -> Bool;\n"
    "     f: Nat # Nat -> Bool;\n"
    "     f: Nat;\n",
    true,
    "Bool"
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_different_arity_horrible_app2)
{
  test_data_expression_in_specification_context(
    "f(f, f)",
    "map  f: Nat -> Bool;\n"
    "     f: Nat # Nat -> Bool;\n"
    "     f: Nat;\n",
    true,
    "Bool"
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_different_arity_horrible_abs)
{
  test_data_expression_in_specification_context(
    "f((lambda x: Bool. f)(f(f, f)))",
    "map  f: Nat -> Bool;\n"
    "     f: Nat # Nat -> Bool;\n"
    "     f: Nat;\n",
    true,
    "Bool"
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_different_arity_functional)
{
  test_data_expression_in_specification_context(
    "f",
    "map  f: Nat -> Nat -> Bool;\n"
    "     f: Nat -> Bool;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_same_arity)
{
  test_data_expression_in_specification_context(
    "f",
    "map  f: Pos -> Nat;\n"
    "     f: Nat -> Pos;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_same_arity_application_nat_constant)
{
  test_data_expression_in_specification_context(
    "f(0)",
    "map  f: Pos -> Nat;\n"
    "     f: Nat -> Pos;\n",
    true,
    "Pos"
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_same_arity_application_pos_constant)
{
  test_data_expression_in_specification_context(
    "f(1)",
    "map  f: Pos -> Nat;\n"
    "     f: Nat -> Pos;\n",
    true,
    "Nat"
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_same_arity_application_nat_variable)
{
  test_data_expression_in_specification_context(
    "f(x)",
    "map  f: Pos -> Nat;\n"
    "     f: Nat -> Pos;\n",
    { var("x", nat()) },
    true,
    "Pos"
  );
}

BOOST_AUTO_TEST_CASE(test_duplicate_function_same_arity_application_pos_variable)
{
  test_data_expression_in_specification_context(
    "f(x)",
    "map  f: Pos -> Nat;\n"
    "     f: Nat -> Pos;\n",
    { var("x", pos()) },
    true,
    "Nat"
  );
}

BOOST_AUTO_TEST_CASE(test_function_symbol)
{
  test_data_expression_in_specification_context(
    "f",
    "map  f: Nat -> Bool;\n",
    true,
    "Nat -> Bool"
  );
}

BOOST_AUTO_TEST_CASE(test_function_application_pos_constant)
{
  test_data_expression_in_specification_context(
    "f(1)",
    "map  f: Nat -> Bool;\n",
    true,
    "Bool"
  );
}

BOOST_AUTO_TEST_CASE(test_function_application_nat_constant)
{
  test_data_expression_in_specification_context(
    "f(0)",
    "map  f: Nat -> Bool;\n",
    true,
    "Bool"
  );
}

BOOST_AUTO_TEST_CASE(test_function_application_int_constant)
{
  test_data_expression_in_specification_context(
    "f(-1)",
    "map  f: Nat -> Bool;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_function_application_pos_variable)
{
  test_data_expression_in_specification_context(
    "f(x)",
    "map  f: Nat -> Bool;\n",
    { var("x", pos()) },
    true,
    "Bool"
  );
}

BOOST_AUTO_TEST_CASE(test_function_application_nat_variable)
{
  test_data_expression_in_specification_context(
    "f(x)",
    "map  f: Nat -> Bool;\n",
    { var("x", nat()) },
    true,
    "Bool"
  );
}

BOOST_AUTO_TEST_CASE(test_function_application_int_variable)
{
  test_data_expression_in_specification_context(
    "f(x)",
    "map  f: Nat -> Bool;\n",
    { var("x", data::sort_int::int_()) },
    false
  );
}

BOOST_AUTO_TEST_CASE(test_struct_constructor)
{
  test_data_expression_in_specification_context(
    "c",
    "sort S = struct c(Nat);\n",
    true,
    "Nat -> S"
  );
}

BOOST_AUTO_TEST_CASE(test_struct_constructor_application_pos_constant)
{
  test_data_expression_in_specification_context(
    "c(1)",
    "sort S = struct c(Nat);\n",
    true,
    "S"
  );
}

BOOST_AUTO_TEST_CASE(test_struct_constructor_application_nat_constant)
{
  test_data_expression_in_specification_context(
    "c(0)",
    "sort S = struct c(Nat);\n",
    true,
    "S"
  );
}

BOOST_AUTO_TEST_CASE(test_struct_constructor_application_int_constant)
{
  test_data_expression_in_specification_context(
    "c(-1)",
    "sort S = struct c(Nat);\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_struct_constructor_application_pos_variable)
{
  test_data_expression_in_specification_context(
    "c(x)",
    "sort S = struct c(Nat);\n",
    { var("x", pos()) },
    true,
    "S"
  );
}

BOOST_AUTO_TEST_CASE(test_struct_constructor_application_nat_variable)
{
  test_data_expression_in_specification_context(
    "c(x)",
    "sort S = struct c(Nat);\n",
    { var("x", nat()) },
    true,
    "S"
  );
}

BOOST_AUTO_TEST_CASE(test_struct_constructor_application_int_variable)
{
  test_data_expression_in_specification_context(
    "c(x)",
    "sort S = struct c(Nat);\n",
    { var("x", data::sort_int::int_()) },
    false
  );
}

BOOST_AUTO_TEST_CASE(test_ambiguous_function)
{
  test_data_expression_in_specification_context(
    "f",
    "sort U;\n"
    "     S;\n"
    "     T;\n\n"
    "map  f: Pos;\n"
    "     f: Pos # Nat -> U;\n"
    "     f: Pos # Pos -> S;\n"
    "     f: Nat # Pos -> T;\n",
    true,
    "Pos"
  );
}

BOOST_AUTO_TEST_CASE(test_ambiguous_function_application1)
{
  test_data_expression_in_specification_context(
    "f(x, x)",
    "sort U;\n"
    "     S;\n"
    "     T;\n\n"
    "map  f: Pos;\n"
    "     f: Pos # Nat -> U;\n"
    "     f: Pos # Pos -> S;\n"
    "     f: Nat # Pos -> T;\n",
    { var("x", pos()), var("y", nat()) },
    true,
    "S"
  );
}

BOOST_AUTO_TEST_CASE(test_ambiguous_function_application2)
{
  test_data_expression_in_specification_context(
    "f(x, y)",
    "sort U;\n"
    "     S;\n"
    "     T;\n\n"
    "map  f: Pos;\n"
    "     f: Pos # Nat -> U;\n"
    "     f: Pos # Pos -> S;\n"
    "     f: Nat # Pos -> T;\n",
    { var("x", pos()), var("y", nat()) },
    true,
    "U"
  );
}

BOOST_AUTO_TEST_CASE(test_ambiguous_function_application3)
{
  test_data_expression_in_specification_context(
    "f(y, x)",
    "sort U;\n"
    "     S;\n"
    "     T;\n\n"
    "map  f: Pos;\n"
    "     f: Pos # Nat -> U;\n"
    "     f: Pos # Pos -> S;\n"
    "     f: Nat # Pos -> T;\n",
    { var("x", pos()), var("y", nat()) },
    true,
    "T"
  );
}

//BOOST_AUTO_TEST_CASE(test_ambiguous_function_application4)
//{
//  test_data_expression_in_specification_context(
//    "f(x, x)",
//    "sort S;\n"
//    "     T;\n"
//    "     U;\n\n"
//    "map  f: Pos;\n"
//    "     f: Pos # Nat -> U;\n"
//    "     f: Nat # Nat -> S;\n"
//    "     f: Nat # Pos -> T;\n",
//    { var("x", pos()), var("y", nat()) },
//    true,
//    "S"
//  );
//}
//
//BOOST_AUTO_TEST_CASE(test_ambiguous_function_application4a)
//{
//  test_data_expression_in_specification_context(
//    "f(x, x)",
//    "sort U;\n"
//    "     S;\n"
//    "     T;\n\n"
//    "map  f: Pos;\n"
//    "     f: Pos # Nat -> U;\n"
//    "     f: Nat # Nat -> S;\n"
//    "     f: Nat # Pos -> T;\n",
//    { var("x", pos()), var("y", nat()) },
//    true,
//    "S"
//  );
//}

BOOST_AUTO_TEST_CASE(test_ambiguous_function_application5)
{
  test_data_expression_in_specification_context(
    "f(x, x)",
    "sort S;\n"
    "     T;\n"
    "     U;\n\n"
    "map  f: Pos;\n"
    "     f: Nat # Nat -> S;\n"
    "     f: Nat # Pos -> T;\n"
    "     f: Pos # Nat -> U;\n",
    { var("x", pos()), var("y", nat()) },
    true,
    "S"
  );
}

BOOST_AUTO_TEST_CASE(test_ambiguous_function_application_recursive)
{
  test_data_expression_in_specification_context(
    "g(f(x))",
    "map  g: Int -> Bool;\n"
    "     f: Pos -> Nat;\n"
    "     f: Pos -> Int;\n",
    { var("x", pos()) },
    false
  );
}

BOOST_AUTO_TEST_CASE(test_ambiguous_function_application_recursive2)
{
  test_data_expression_in_specification_context(
    "g(f(x))",
    "map  g: Int -> Bool;\n"
    "     f: Pos -> Nat;\n"
    "     f: Pos -> Int;\n"
    "     g: Int -> Int;\n",
    { var("x", pos()) },
    false
  );
}

BOOST_AUTO_TEST_CASE(test_ambiguous_function_application_recursive3)
{
  test_data_expression_in_specification_context(
    "g(f(x))",
    "map  g: Int -> Bool;\n"
    "     f: Pos -> Nat;\n"
    "     f,g: Int -> Int;\n",
    { var("x", pos()) },
    false,
    "Bool"
  );
}

BOOST_AUTO_TEST_CASE(test_ambiguous_function_application_recursive4)
{
  test_data_expression_in_specification_context(
    "g(f(x))",
    "map  g: Int -> Bool;\n"
    "     f: Pos -> Nat;\n"
    "     f: Pos -> Int;\n"
    "     g: Nat -> Int;\n",
    { var("x", pos()) },
    false
  );
}

BOOST_AUTO_TEST_CASE(test_aliases)
{
  test_data_expression_in_specification_context(
    "f == g",
    "sort B;\n"
    "     A = List(List(B));\n"
    "     C = List(B);\n",
    { var("f", data::basic_sort("A")), var("g", list(data::basic_sort("C"))) },
    true
  );
}

BOOST_AUTO_TEST_CASE(test_bag_with_pos_as_argument)
{
  test_data_expression_in_specification_context(
    "{ n: Pos | n + 1 }",
    "sort dummy;\n",
    { },
    true,
    "Bag(Pos)"
  );
}

BOOST_AUTO_TEST_CASE(test_bag_with_nat_as_argument1)
{
  test_data_expression_in_specification_context(
    "{ n: Pos | 0 }",
    "sort dummy;\n",
    { },
    true,
    "Bag(Pos)"
  );
}

BOOST_AUTO_TEST_CASE(test_bag_with_nat_as_argument2)
{
  test_data_expression_in_specification_context(
    "{ n: Nat | n }",
    "sort dummy;\n",
    { },
    true,
    "Bag(Nat)"
  );
}

BOOST_AUTO_TEST_CASE(test_bag_with_real_as_argument)
{
  test_data_expression_in_specification_context(
    "{ n: Pos | 2 / 3 }",
    "sort dummy;\n",
    data::variable_vector(),
    false
  );
}

/* The test below shows an ambiguous projection function that
 * cannot be resolved with the current typechecker. This test should
 * be enabled with a new typechecker. */
BOOST_AUTO_TEST_CASE(test_ambiguous_projection_function)
{
  test_data_expression_in_specification_context(
    "R(pi_1(p)) && IS_T1(p)",
    "sort S;\n"
    "     T = struct T0 | T1(pi_1: T)?IS_T1 | T2(pi_1: S)?IS_T2;\n\n"
    "map  R: T -> Bool;\n",
    { var("p", data::basic_sort("T")) },
    false     // <-------------- Should be set to true with a new typechecker ---------------------------------------
  );
}


BOOST_AUTO_TEST_CASE(test_lambda_term_with_wrong_number_of_arguments)
{
  /* The typechecker couldn't catch the wrongly typed term below in november 2012,
     which led to a core dump */
  test_data_expression("((lambda x: Nat. x)(1, 2) > 0)",false);
}

/* The example below has the nasty feature that the sort of
   # in the expression below can be #:List(Nat)->Nat,
      List(Int)->Nat and List(Real)->Nat. In version 10169 of
      the toolset the type of # became List(PossibleTypes([Nat, Int, Real])
      causing confusion in the other tools */
BOOST_AUTO_TEST_CASE(test_avoidance_of_possible_types)
{
  test_data_expression_in_specification_context(
    "#[0, 1] == -1",
    "sort dummy;\n",
    { },
    true,
    "Bool"
  );
}

/* The next example checks whether Int2Pos is properly typed. */
BOOST_AUTO_TEST_CASE(test_proper_use_of_int2pos)
{
  test_data_expression_in_specification_context(
    "f(Int2Pos(-1))",
    "map  f: Pos -> Bool;\n",
    { },
    true,
    "Bool"
  );
}

/* This example checks whether explicit transformations among
 * numbers are properly typable.*/
BOOST_AUTO_TEST_CASE(test_proper_use_of_int2pos1)
{
  test_data_expression_in_specification_context(
    "fpos(Nat2Pos(0)) && fpos(Int2Pos(-1)) && fpos(Real2Pos(1 / 2)) && "
    "fnat(Int2Nat(-1)) && fnat(Real2Nat(1 / 2)) && "
    "fint(Real2Int(1 / 2))",
    "map  fpos: Pos -> Bool;\n"
    "     fnat: Nat -> Bool;\n"
    "     fint: Int -> Bool;\n",
    { },
    true,
    "Bool"
  );
}

class testable_sort_type_checker: public data::sort_type_checker
{
  public:
    std::pair<bool, bool> check_alias(const data::alias& x)
    {
      // search for the alias x
      // auto x_iter = m_normalized_aliases.end();
      // for (auto i = m_normalized_aliases.begin(); i != m_normalized_aliases.end(); ++i)
      bool found=false;
      for (const data::alias& a: get_sort_specification().user_defined_aliases())
      {
        if (a == x)
        {
          found=true;
          break;
        }
      }
      // if (x_iter == m_normalized_aliases.end())
      if (!found)
      {
        throw mcrl2::runtime_error("could not find alias " + data::pp(x));
      }

      std::map < data::basic_sort, data::sort_expression > alias_map;
      for(const data::alias& a: get_sort_specification().user_defined_aliases())
      {
        alias_map[a.name()]=a.reference();
      }
      std::set<data::basic_sort> sort_already_seen;

      bool first, second;
      try
      {
        first = true;

        // check_alias_recursion(x.name(), x.reference());
        check_for_sort_alias_loop_through_function_sort(x.name(),x.reference(),sort_already_seen, false, alias_map);
        assert(sort_already_seen.size()==0);
      }
      catch (mcrl2::runtime_error& e)
      {
        mCRL2log(log::debug) << e.what() << std::endl;
        first = false;
      }
      try
      {
        second = true;
        // check_alias_circularity(x.name(), x.reference());
        check_alias_circularity(x.name(), x.reference(),sort_already_seen, alias_map);
        assert(sort_already_seen.size()==0);
      }
      catch (mcrl2::runtime_error& e)
      {
        mCRL2log(log::debug) << e.what() << std::endl;
        second = false;
      }
      return std::make_pair(first, second);
    }

    /// \brief constructs a sort expression checker.
    testable_sort_type_checker(const data::sort_specification sort_spec)
      : data::sort_type_checker(sort_spec, false)
    {}

};

BOOST_AUTO_TEST_CASE(test_sort_aliases)
{
  std::pair<data::basic_sort_vector, data::alias_vector> sortspec = data::parse_sort_specification(
      "sort                           \n"
      "  A;                           \n"
      "  B;                           \n"
      "  C;                           \n"
      "  A1  = A1;                    \n"
      "  A2  = List(A2);              \n"
      "  A3  = Set(A3);               \n"
      "  A4  = Bag(A4);               \n"
      "  A5  = A5 -> B;               \n"
      "  A6  = B -> A6;               \n"
      "  A7  = struct f(A7);          \n"
      "  A8  = struct f(Nat -> A8);   \n"
      "  A9  = struct f(x: List(A9)); \n"
      "  A10 = List(struct f(A10));   \n"
      "  A11 = struct A11 | B;        \n"
      "  A12 = FSet(A12);             \n"
      "  A13 = FBag(A13);             \n"
      "  A14 = struct f(FSet(A14)) | c;\n"
      "  A15 = struct f(FSet(A15)) | g(FBag(A15)) | c;\n"
  );

  std::string expected_results(
      "  A1  true false  \n"
      "  A2  true false  \n"
      "  A3  false false  \n"
      "  A4  false false  \n"
      "  A5  false false  \n"
      "  A6  false false  \n"
      "  A7  true true   \n"
      "  A8  false true  \n"
      "  A9  true true   \n"
      "  A10 true true   \n"
      "  A11 true true   \n"
      "  A12 true false  \n"
      "  A13 true false  \n"
      "  A14 true true  \n"
      "  A15 true true  \n"
  );

  std::map<std::string, std::pair<bool, bool> > expected_result_map;
  for (std::string line: utilities::regex_split(expected_results, "\\n"))
  {
    auto words = utilities::regex_split(utilities::trim_copy(line), "\\s+");
    if (words.size() == 3)
    {
      std::string name = words[0];
      bool result1 = words[1] == "true";
      bool result2 = words[2] == "true";
      expected_result_map[name] = std::make_pair(result1, result2);
    }
  }

  data::sort_specification sp(sortspec.first,sortspec.second);
  testable_sort_type_checker checker(sp);
  for (const data::alias& a: sortspec.second)
  {
    std::pair<bool, bool> result = checker.check_alias(a);
    std::string name = core::pp(a.name().name());
    std::pair<bool, bool> expected_result = expected_result_map[name];
    if (result != expected_result)
    {
      std::clog << "ERROR: alias " << a
                << " result = " << std::boolalpha << result.first << " " << std::boolalpha << result.second
                << " expected result = " << std::boolalpha << expected_result.first << " " << std::boolalpha << expected_result.second
                << std::endl;
    }
    BOOST_CHECK(result == expected_result);
  }
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
