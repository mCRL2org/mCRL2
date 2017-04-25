// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file type_check_tree_test.cpp
/// \brief Add your file description here.

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/experimental/type_check_tree.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/type_checker.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/untyped_sort.h"
#include "mcrl2/utilities/text_utility.h"
#include <boost/test/included/unit_test_framework.hpp>
#include <iostream>
#include <sstream>

using namespace mcrl2;

void test_data_expression(const std::string& text, const std::string& variable_context = "", const std::string& expected_sort = "", bool expected_result = true)
{
  std::cout << "--------------------------------------------------------------------" << std::endl;
  std::cout << "text            = " << text << std::endl;
  std::cout << "variables       = " << variable_context << std::endl;
  std::cout << "expected sort   = " << expected_sort << std::endl;
  std::cout << "expected result = " << std::boolalpha << expected_result << std::endl;
  data::data_specification dataspec;
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("DataExpr");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  data::type_check_context context;
  data::variable_list variables = data::parse_variables(variable_context);
  data::type_check_node_ptr tnode = data::type_check_tree_generator(context, p).parse_DataExpr(node);
  context.add_context_variables(variables);
  tnode->set_constraint(context);
  data::print_node(tnode);
  context.remove_context_variables(variables);
  p.destroy_parse_node(node);
}

void test_data_expression_fail(const std::string& text, const std::string& variable_context = "", const std::string& expected_sort = "")
{
  test_data_expression(text, variable_context, expected_sort, false);
}

BOOST_AUTO_TEST_CASE(data_expressions_test)
{
  test_data_expression("x"                                                     , "x: Pos;"                            , "Pos"                        );
  test_data_expression("true"                                                  , ""                                   , "Bool"                       );
  test_data_expression("if(true, true, false)"                                 , ""                                   , "Bool"                       );
  test_data_expression("!true"                                                 , ""                                   , "Bool"                       );
  test_data_expression("true && false"                                         , ""                                   , "Bool"                       );
  test_data_expression("0"                                                     , ""                                   , "Nat"                        );
  test_data_expression("-1"                                                    , ""                                   , "Int"                        );
  test_data_expression("0 + 1"                                                 , ""                                   , "Pos"                        );
  test_data_expression("1 + 0"                                                 , ""                                   , "Pos"                        );
  test_data_expression("0 + 0"                                                 , ""                                   , "Nat"                        );
  test_data_expression("1 + 1"                                                 , ""                                   , "Pos"                        );
  test_data_expression("1 * 2 + 3"                                             , ""                                   , "Pos"                        );
  test_data_expression("[]"                                                    , ""                                   , ""                           );
  test_data_expression("[] ++ []"                                              , ""                                   , ""                           );
  test_data_expression("#[]"                                                   , ""                                   , "Nat"                        );
  test_data_expression("true in []"                                            , ""                                   , "Bool"                       );
  test_data_expression("[true, false]"                                         , ""                                   , "List(Bool)"                 );
  test_data_expression("[0]"                                                   , ""                                   , "List(Nat)"                  );
  test_data_expression("[1, 2]"                                                , ""                                   , "List(Pos)"                  );
  test_data_expression("[0] ++ [1, 2]"                                         , ""                                   , "List(Nat)"                  );
  test_data_expression("[0, 1, 2]"                                             , ""                                   , "List(Nat)"                  );
  test_data_expression("[1, 0, 2]"                                             , ""                                   , "List(Nat)"                  );
  test_data_expression("l ++ [1, 2]"                                           , "l: List(Nat);"                      , "List(Nat)"                  );
  test_data_expression("l ++ [1, 2]"                                           , "l: List(Pos);"                      , "List(Pos)"                  );
  test_data_expression("[0] ++ l"                                              , "l: List(Nat);"                      , "List(Nat)"                  );
  test_data_expression("{}"                                                    , ""                                   , ""                           );
  test_data_expression("!{}"                                                   , ""                                   , ""                           );
  test_data_expression("!{} <= {}"                                             , ""                                   , ""                           );
  test_data_expression("{} <= !{}"                                             , ""                                   , ""                           );
  test_data_expression("{ true, false }"                                       , ""                                   , "FSet(Bool)"                 );
  test_data_expression("{ 1, 2, -7 }"                                          , ""                                   , "FSet(Int)"                  );
  test_data_expression("{ x: Nat | x mod 2 == 0 }"                             , ""                                   , "Set(Nat)"                   );
  test_data_expression("{:}"                                                   , ""                                   , ""                           );
  test_data_expression("{ true: 1, false: 2 }"                                 , ""                                   , "FBag(Bool)"                 );
  test_data_expression("{ 1: 1, 2: 2, -8: 8 }"                                 , ""                                   , "FBag(Int)"                  );
  test_data_expression("{ x: Nat | (lambda y: Nat. y * y)(x) }"                , ""                                   , "Bag(Nat)"                   );
  test_data_expression("(lambda x: Bool. x)[true -> false]"                    , ""                                   , ""                           );
  test_data_expression("(lambda x: Bool. x)[true -> false][false -> true]"     , ""                                   , ""                           );
  test_data_expression("(lambda n: Nat. n mod 2 == 0)[0 -> false]"             , ""                                   , ""                           );
  test_data_expression("lambda x,y: struct t. x == y"                          , ""                                   , "struct t # struct t -> Bool");
  test_data_expression("forall x,y: struct t. x == y"                          , ""                                   , "Bool"                       );
  test_data_expression("forall n: Nat. n >= 0"                                 , ""                                   , "Bool"                       );
  test_data_expression("forall n: Nat. n > -1"                                 , ""                                   , "Bool"                       );
  test_data_expression("exists x,y: struct t. x == y"                          , ""                                   , "Bool"                       );
  test_data_expression("exists n: Nat. n > 481"                                , ""                                   , "Bool"                       );
  test_data_expression("x == y"                                                , "x: struct t?is_t; y: struct t?is_t;", "Bool"                       );
  test_data_expression("x whr x = 3 end"                                       , "x: Nat;"                            , "Nat"                        );
  test_data_expression("x + y whr x = 3, y = 10 end"                           , ""                                   , "Pos"                        );
  test_data_expression("x + y whr x = 3, y = 10 end"                           , "x: Pos; y: Nat;"                    , "Pos"                        );
  test_data_expression("x + y whr x = 3, y = x + 10 end"                       , "x: Pos; y: Nat;"                    , "Pos"                        );
  test_data_expression("x + y whr x = 3, y = x + y + 10 end"                   , "x: Pos; y: Nat;"                    , "Pos"                        );
  test_data_expression("x + y whr x = y + 10, y = 3 end"                       , "x: Pos; y: Nat;"                    , "Pos"                        );
  test_data_expression("x + y whr x = y + 10, y = x + 3 end"                   , "x: Pos; y: Nat;"                    , "Pos"                        );
  test_data_expression("x ++ y whr x = [0, y], y = [x] end"                    , "x: Nat; y: Nat;"                    , "List(Nat)"                  );
  test_data_expression("x + y"                                                 , "x: Pos; y: Nat;"                    , "Pos"                        );
  test_data_expression("x == y"                                                , "x: Pos; y: Nat;"                    , "Bool"                       );

  test_data_expression_fail("[0] ++ l"                                         , "l: List(Pos);"                 );
  test_data_expression_fail("x ++ y"                                           , "x: List(Pos); y: List(Pos);"   );
  test_data_expression_fail("x == y"                                           , "x: List(Pos); y: List(Nat);"   );
  test_data_expression_fail("!{:}"                                             , ""                              );
  test_data_expression_fail("(lambda x: Bool. x)[0 -> false]"                  , ""                              );
  test_data_expression_fail("lambda x: struct t?is_t. x == t"                  , ""                              );
  test_data_expression_fail("lambda x: struct t. x == t"                       , ""                              );
  test_data_expression_fail("lambda x: struct t?is_t, y: struct t. x == y"     , ""                              );
  test_data_expression_fail("lambda f: Nat. lambda f: Nat -> Bool. f(f)"       , ""                              );
  test_data_expression_fail("x == y"                                           , "x: struct t; y: struct t?is_t;");
  test_data_expression_fail("x + y whr x = 3, y = x + 10 end"                  , ""                              );
  test_data_expression_fail("x + y whr x = 3, y = x + y + 10 end"              , ""                              );
  test_data_expression_fail("x + y whr x = y + 10, y = 3 end"                  , ""                              );
  test_data_expression_fail("x + y whr x = y + 10, y = x + 3 end"              , ""                              );
  test_data_expression_fail("x ++ y whr x = [0, y], y = [x] end"               , "x: Pos; y: Nat;"               );
}

BOOST_AUTO_TEST_CASE(replace_untyped_sort_test)
{
  using namespace mcrl2::data;
  const sort_expression& B = sort_bool::bool_();
  const sort_expression& U = untyped_sort();
  data_expression s1 = sort_set::intersection(U, sort_fset::fset(U), sort_fset::fset(U));
  data_expression s2 = sort_set::intersection(B, sort_fset::fset(B), sort_fset::fset(B));
  BOOST_CHECK(has_untyped_sort(s1));
  BOOST_CHECK(!has_untyped_sort(s2));
  BOOST_CHECK(replace_untyped_sort(s1, B) == s2);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
