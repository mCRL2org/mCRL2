// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <set>
#include <sstream>
#include <stack>

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/detail/concepts.h"
#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/optimized_boolean_operators.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/substitutions/mutable_indexed_substitution.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"

using namespace mcrl2;
using namespace mcrl2::data;

// Example: parse_variable_list("x:Nat; y:Pos");
variable_list parse_variable_list(const std::string& text, const data_specification& dataspec = detail::default_specification())
{
  variable_vector result;
  parse_variables(text, std::back_inserter(result), dataspec);
  return variable_list(result.begin(), result.end());
}

void enumerate(const data_specification& dataspec,
               const variable_list& variables,
               const data_expression& expression,
               std::size_t expected_no_of_solutions,
               bool more_solutions_possible)
{
  typedef enumerator_list_element_with_substitution<> enumerator_element;
  typedef enumerator_algorithm_with_iterator<> enumerator_type;

  data::enumerator_identifier_generator id_generator;
  rewriter rewr(dataspec);
  enumerator_type enumerator(rewr, dataspec, rewr, id_generator);
  std::size_t number_of_solutions = 0;
  mutable_indexed_substitution<> sigma;
  std::deque<enumerator_element> enumerator_deque(1, enumerator_element(variables, expression));
  auto i = enumerator.begin(sigma, enumerator_deque);
  for ( ; number_of_solutions < expected_no_of_solutions && i != enumerator.end(); ++i)
  {
    mutable_map_substitution<> rho;
    i->add_assignments(variables, rho, rewr);
    std::cout << "solution = " << rho << std::endl;
    number_of_solutions++;
  }
  BOOST_CHECK(number_of_solutions == expected_no_of_solutions && (more_solutions_possible || i == enumerator.end()));
}

void enumerate(const std::string& dataspec_text,
               const std::string& variable_text,
               const std::string& expression_text,
               const std::string& free_variable_text,
               std::size_t number_of_solutions,
               bool more_solutions_possible)
{
  data_specification dataspec = parse_data_specification(dataspec_text);
  variable_list variables = parse_variable_list(variable_text, dataspec);
  variable_list free_variables = parse_variable_list(free_variable_text, dataspec);
  data_expression expression = parse_data_expression(expression_text, free_variables, dataspec);
  enumerate(dataspec, variables, expression, number_of_solutions, more_solutions_possible);
}

BOOST_AUTO_TEST_CASE(empty_test)
{
  typedef enumerator_list_element_with_substitution<> enumerator_element;
  typedef enumerator_algorithm_with_iterator<> enumerator_type;

  // test manual construction of rewr with rewriter
  data_specification dataspec;
  rewriter rewr(dataspec);
  variable_list variables;

  std::size_t count = 0;
  data::enumerator_identifier_generator id_generator;

  // explicit with condition rewr and condition
  enumerator_type enumerator(rewr, dataspec, rewr, id_generator);

  mutable_indexed_substitution<> sigma;
  std::deque<enumerator_element> enumerator_deque(1, enumerator_element(variables, sort_bool::true_()));
  for (auto i = enumerator.begin(sigma, enumerator_deque); i != enumerator.end(); ++i)
  {
    count++;
  }
  BOOST_CHECK(count == 1);

  // explicit with condition but without condition rewr
  enumerator_deque.clear();
  enumerator_deque.emplace_back(enumerator_element(variables, sort_bool::true_()));
  for (auto i = enumerator.begin(sigma, enumerator_deque); i != enumerator.end(); ++i)
  {
    count++;
  }
  BOOST_CHECK(count == 2);

  variables = parse_variable_list("y: Nat;");
  enumerator_deque.clear();
  enumerator_deque.emplace_back(enumerator_element(variables, sort_bool::false_()));
  for (auto i = enumerator.begin(sigma, enumerator_deque); i != enumerator.end(); ++i)
  {
    BOOST_CHECK(false);
  }
}

BOOST_AUTO_TEST_CASE(list_test)
{
  std::string dataspec_text =
    "sort list_of_booleans;                                    \n"
    "cons empty : list_of_booleans;                            \n"
    "     lcons : Bool # list_of_booleans -> list_of_booleans; \n"
    "map  size   : list_of_booleans -> Nat;                    \n"
    "var  l : list_of_booleans;                                \n"
    "     b : Bool;                                            \n"
    "eqn  size(empty) = 0;                                     \n"
    "     size(lcons(b,l)) = 1 + size(l);                      \n"
    ;
  std::string variable_text = "x: list_of_booleans; y: Nat;";
  std::string expression_text = "y == size(x) && 0 < y && y < 2";
  std::string free_variable_text = "x : list_of_booleans; y : Nat;";
  std::size_t number_of_solutions = 0;
  bool more_solutions_possible = true; // Changed!
  enumerate(dataspec_text, variable_text, expression_text, free_variable_text, number_of_solutions, more_solutions_possible);
}

BOOST_AUTO_TEST_CASE(structured_sort_test)
{
  std::string dataspec_text =
    "sort D = struct d1(E) | d2(E); \n"
    "     E = struct e1 | e2;       \n"
    ;
  std::string variable_text = "d: D;";
  std::string expression_text = "forall d: D. d == e";
  std::string free_variable_text = "e: D;";
  std::size_t number_of_solutions = 4;
  bool more_solutions_possible = false;
  enumerate(dataspec_text, variable_text, expression_text, free_variable_text, number_of_solutions, more_solutions_possible);
}

BOOST_AUTO_TEST_CASE(equality_substitution_test)
{
  std::string dataspec_text = "sort L = Nat;";
  std::string variable_text = "";
  std::string expression_text = "x == 17 && x != 17";
  std::string free_variable_text = "x : Pos;";
  std::size_t number_of_solutions = 0;
  bool more_solutions_possible = true; // Changed!

  std::clog << "Test1 equality\n";
  enumerate(dataspec_text, variable_text, expression_text, free_variable_text, number_of_solutions, more_solutions_possible);

  std::clog << "Test2 equality\n";
  expression_text = "x == 17 && x == x";
  number_of_solutions = 1;
  more_solutions_possible = false;
  enumerate(dataspec_text, variable_text, expression_text, free_variable_text, number_of_solutions, more_solutions_possible);

  std::clog << "Test3 equality\n";
  expression_text = "x == 17 && 2*x == 34";
  enumerate(dataspec_text, variable_text, expression_text, free_variable_text, number_of_solutions, more_solutions_possible);

  std::clog << "Test4 equality\n";
  variable_text = "";
  number_of_solutions = 1;
  enumerate(dataspec_text, variable_text, expression_text, free_variable_text, number_of_solutions, more_solutions_possible);

  std::clog << "Test4 equality: return two non exact solutions\n";
  variable_text = "b: Bool;";
  number_of_solutions = 2;
  enumerate(dataspec_text, variable_text, expression_text, free_variable_text, number_of_solutions, more_solutions_possible);
}

BOOST_AUTO_TEST_CASE(tree_test)
{
  std::string dataspec_text =
    "sort tree_with_booleans;                                                   \n"
    "cons leaf : Bool -> tree_with_booleans;                                    \n"
    "cons node : tree_with_booleans # tree_with_booleans -> tree_with_booleans; \n"
    ;
  std::string variable_text = "x : tree_with_booleans;";
  std::string expression_text = "true";
  std::string free_variable_text = "";
  std::size_t number_of_solutions = 32;
  bool more_solutions_possible = true;
  enumerate(dataspec_text, variable_text, expression_text, free_variable_text, number_of_solutions, more_solutions_possible);
}

BOOST_AUTO_TEST_CASE(mutually_recursive_test)
{
  std::string dataspec_text =
    "sort this;                                              \n"
    "     that;                                              \n"
    "cons a : this;                                          \n"
    "     A : that -> this;                                  \n"
    "     b : that;                                          \n"
    "     B : this -> that;                                  \n"
    "map  maximum_a : Nat # this -> Bool;                    \n"
    "     maximum_a : Nat # that -> Bool;                    \n"
    "     maximum_b : Nat # this -> Bool;                    \n"
    "     maximum_b : Nat # that -> Bool;                    \n"
    "var  x : Nat;                                           \n"
    "     ax : this;                                         \n"
    "     bx : that;                                         \n"
    "eqn  maximum_a(x,A(bx)) = maximum_a(Int2Nat(x - 1),bx); \n"
    "     maximum_a(0,a) = true;                             \n"
    "     maximum_b(x,B(ax)) = maximum_b(Int2Nat(x - 1),ax); \n"
    "     maximum_b(0,b) = true;                             \n"
    ;

  std::string variable_text = "x : this;";
  std::string expression_text = "true";
  std::string free_variable_text = "";
  std::size_t number_of_solutions = 32;
  bool more_solutions_possible = true;
  enumerate(dataspec_text, variable_text, expression_text, free_variable_text, number_of_solutions, more_solutions_possible);

  variable_text = "x : that;";
  enumerate(dataspec_text, variable_text, expression_text, free_variable_text, number_of_solutions, more_solutions_possible);
}

BOOST_AUTO_TEST_CASE(set_test)
{
  std::string dataspec_text = "sort Dummy;\n";
  std::string variable_text = "f: Set(Bool);";
  std::string expression_text = "true in f";
  std::string free_variable_text = "f: Set(Bool);";
  std::size_t number_of_solutions = 4;
  bool more_solutions_possible = false;
  enumerate(dataspec_text, variable_text, expression_text, free_variable_text, number_of_solutions, more_solutions_possible);
}

inline
data_expression_vector generate_values(const data_specification& dataspec, const sort_expression& s, std::size_t max_size = 1000)
{
  typedef enumerator_list_element_with_substitution<> enumerator_element;
  typedef enumerator_algorithm_with_iterator<> enumerator_type;

  std::size_t max_internal_variables = 10000;
  data_expression_vector result;
  data::enumerator_identifier_generator id_generator;

  rewriter rewr(dataspec);
  enumerator_type enumerator(rewr, dataspec, rewr, id_generator, max_internal_variables);
  variable v("x", s);
  variable_list variables;
  variables.push_front(v);
  mutable_indexed_substitution<> sigma;
  std::deque<enumerator_element> enumerator_deque(1, enumerator_element(variables, sort_bool::true_()));
  for (auto i = enumerator.begin(sigma, enumerator_deque); i != enumerator.end() ; ++i)
  {
    i->add_assignments(variables, sigma, rewr);
    result.push_back(sigma(variables.front()));
    if (result.size() >= max_size)
    {
      break;
    }
  }
  return result;
}

bool no_duplicates(const data_expression_vector& v)
{
  std::set<data_expression> s;
  s.insert(v.begin(), v.end());
  return s.size() == v.size();
}

BOOST_AUTO_TEST_CASE(generate_values_test)
{
  std::string DATASPEC =
    "sort Identifiers = struct t1 | t2 | t3 | t4 | t5 | p1 | p2 | s1 | r1 | r2 | r3 | IL;                                                                                  \n"
    "     signal_Messages = struct ic_set_proceed_signal | ic_set_stop_signal;                                                                                             \n"
    "     track_Messages = struct dv_free_track | dv_occupied_track;                                                                                                       \n"
    "     point_Messages = struct ic_move_right_point | ic_move_left_point | dv_at_right_point | dv_at_left_point;                                                         \n"
    "     route_Messages = struct ic_cancel_route_route | ic_set_route_route;                                                                                              \n"
    "     Interlocking_Messages = struct c_set_stop | c_move_left | c_set_route | c_IL_off | c_cancel_route | tc_set_proceed | c_IL_on | c_move_right;                     \n"
    "     environment_Messages = struct i_signal_status_proceed_signal_environment | i_signal_status_stop_signal_environment | i_track_status_occupied_track_environment | \n"
    "       i_track_status_free_track_environment | i_point_at_right_point_environment | i_point_locked_point_environment | i_point_ok_point_environment |                 \n"
    "       i_point_broken_point_environment | i_point_at_left_point_environment | i_route_cancelled_route_environment | i_route_established_route_environment;            \n"
    "     rail_yard_Messages = struct sv_set_proceed_signal_railyard | sv_set_stop_signal_railyard | sv_move_right_point_railyard | sv_move_left_point_railyard;           \n"
    "     Enum3 = struct e2_3 | e1_3 | e0_3;                                                                                                                               \n"
    "     track__ready_States = struct track__ready_free_substate | track__ready_occupied_substate | track__ready_startup_substate | track__ready_nop;                     \n"
    "     Enum15 = struct e14_15 | e13_15 | e12_15 | e11_15 | e10_15 | e9_15 | e8_15 | e7_15 | e6_15 | e5_15 | e4_15 | e3_15 | e2_15 | e1_15 | e0_15;                      \n"
    "     point_States = struct point__broken_substate | point__startup_substate | point__working_substate;                                                                \n"
    "     point__working_States = struct point__working_right_substate | point__working_left_substate | point__working_moving_substate | point__working_nop;               \n"
    "     point__working_moving_States = struct point__working_moving_left_substate | point__working_moving_right_substate | point__working_moving_nop;                    \n"
    "     Enum7 = struct e6_7 | e5_7 | e4_7 | e3_7 | e2_7 | e1_7 | e0_7;                                                                                                   \n"
    "     signal_States = struct signal__stop_substate | signal__proceed_substate;                                                                                         \n"
    "     Enum10 = struct e9_10 | e8_10 | e7_10 | e6_10 | e5_10 | e4_10 | e3_10 | e2_10 | e1_10 | e0_10;                                                                   \n"
    "     route_States = struct route__idle_substate | route__established_substate | route__setting_up_substate;                                                           \n"
    "     route__established_States = struct route__established_in_use_substate | route__established_active_substate | route__established_nop;                             \n"
    "     Enum11 = struct e10_11 | e9_11 | e8_11 | e7_11 | e6_11 | e5_11 | e4_11 | e3_11 | e2_11 | e1_11 | e0_11;                                                          \n"
    "     Enum13 = struct e12_13 | e11_13 | e10_13 | e9_13 | e8_13 | e7_13 | e6_13 | e5_13 | e4_13 | e3_13 | e2_13 | e1_13 | e0_13;                                        \n"
    "     HAL_device_States = struct HAL_device__normal_substate;                                                                                                          \n"
    "     track_States = struct track__ready_substate;                                                                                                                     \n"
    ;
  data_specification dataspec = parse_data_specification(DATASPEC);

  for (const sort_expression& sort: dataspec.user_defined_sorts())
  {
    sort_expression s = normalize_sorts(sort, dataspec);
    std::clog << "--- sort " << data::pp(s) << std::endl;
    data_expression_vector v = generate_values(dataspec, s, 10);
    std::clog << " possible values: " << core::detail::print_set(v) << std::endl;
    BOOST_CHECK(v.size() <= 10);
    BOOST_CHECK(no_duplicates(v));
  }

  for (const alias& a: dataspec.user_defined_aliases())
  {
    sort_expression s = normalize_sorts(a.reference(),dataspec);
    std::clog << "--- sort " << data::pp(s) << std::endl;
    data_expression_vector v = generate_values(dataspec, s, 10);
    std::clog << " possible values: " << core::detail::print_set(v) << std::endl;
    BOOST_CHECK(v.size() <= 10);
    BOOST_CHECK(no_duplicates(v));
  }
}

BOOST_AUTO_TEST_CASE(constructors_that_are_not_a_normal_form_test)
{
  std::string dataspec_text =
    "sort FloorID     = struct F1 | F2;\n"
    " \n"
    "map  fSucc      : FloorID           -> FloorID;\n"
    "     equal      : FloorID # FloorID      -> Bool;\n"
    "\n"
    "var  f,g       : FloorID;\n"
    "eqn  F2               = fSucc(F1); \n"
    "     equal(fSucc(f),fSucc(g))  = equal(f,g);\n"
    "     equal(fSucc(f),F1)      = false;\n"
    "     equal(F1,fSucc(g))      = false;\n"
    "     equal(F1,F1)        = true;\n"
    "     f==g            = equal(f,g);\n"
    ;
  std::string variable_text = "f: FloorID;";
  std::string expression_text = "equal(f, F2)";
  const std::string& free_variable_text = variable_text;
  std::size_t number_of_solutions = 1;
  bool more_solutions_possible = false;
  enumerate(dataspec_text, variable_text, expression_text, free_variable_text, number_of_solutions, more_solutions_possible);
}

BOOST_AUTO_TEST_CASE(cannot_enumerate_real_default)
{
  typedef data::enumerator_list_element<data_expression> enumerator_element;

  data_expression result = data::sort_bool::true_();
  data::variable_list v  = { data::variable("r", data::sort_real::real_()) };
  data_expression phi = parse_data_expression("r == r", v);
  data::data_specification dataspec;
  dataspec.add_context_sort(data::sort_real::real_());
  data::rewriter R(dataspec);
  data::enumerator_identifier_generator id_generator("x");
  data::enumerator_algorithm<> E(R, dataspec, R, id_generator, (std::numeric_limits<std::size_t>::max)(), true);
  std::deque<enumerator_element> P;
  data::rewriter::substitution_type sigma;

  try
  {
    P.push_back(enumerator_element(v, phi));
    E.next(P, sigma, data::is_not_true());
    while (!P.empty())
    {
      result = data::optimized_and(result, P.front().expression());
      P.pop_front();
      if (result == data::sort_bool::false_())
      {
        break;
      }
      E.next(P, sigma, data::is_not_true());
    }
  }
  catch (mcrl2::runtime_error& e)
  {
    std::cout << e.what() << std::endl;
    return;
  }
  BOOST_CHECK(false);
}

BOOST_AUTO_TEST_CASE(cannot_enumerate_real_with_substitution)
{
  typedef data::enumerator_list_element_with_substitution<data_expression> enumerator_element;

  data::data_specification dataspec;
  dataspec.add_context_sort(data::sort_real::real_());
  data::rewriter R(dataspec);
  data::variable_list v;
  v.push_front(data::variable("r", data::sort_real::real_()));
  data_expression phi = parse_data_expression("r == r", v);
  data::mutable_indexed_substitution<> sigma;
  std::size_t max_count = 1000;
  bool throw_exceptions = true;
  data::enumerator_identifier_generator id_generator;
  data::enumerator_algorithm_with_iterator<rewriter, enumerator_element, enumerator_identifier_generator> E(R, dataspec, R, id_generator, max_count, throw_exceptions);
  std::deque<enumerator_element> P;
  P.push_back(enumerator_element(v, phi));
  try {
    for (auto i = E.begin(sigma, P); i != E.end() ; ++i)
    {
      // skip
    }
  }
  catch (mcrl2::runtime_error& e)
  {
    std::cout << e.what() << std::endl;
    return;
  }
  BOOST_CHECK(false);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
