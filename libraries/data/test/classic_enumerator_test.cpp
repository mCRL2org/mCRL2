// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <set>
#include <sstream>
#include <stack>

#include <boost/test/minimal.hpp>

#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/detail/concepts.h"
#include "mcrl2/data/standard_utility.h"

using namespace mcrl2;
using namespace mcrl2::data;

void enumerate(const data_specification & d,
               const std::set< variable > & v,
               const data_expression & c,
               const size_t expected_no_of_solutions,
               const bool more_solutions_possible=false)
{

  typedef classic_enumerator< rewriter > enumerator_type;

  rewriter evaluator(d);
  enumerator_type enumerator(d,evaluator);
  size_t number_of_solutions=0;
  enumerator_type::iterator i=enumerator.begin(v, c);
  for ( ; number_of_solutions< expected_no_of_solutions && i != enumerator.end(); ++i)
  {
    number_of_solutions++;
  }
  BOOST_CHECK(number_of_solutions==expected_no_of_solutions && (!more_solutions_possible || i==enumerator.end()));
}

void enumerate(const std::string &specification,
               const std::string &enum_variables,
               const std::string &condition,
               const std::string &free_variables,
               const size_t number_of_solutions,
               const bool more_solutions_possible=false)
{
  data_specification data_spec(parse_data_specification(specification));
  std::set < variable > enum_vars;
  parse_variables(enum_variables, inserter(enum_vars,enum_vars.begin()),data_spec);
  const data_expression cond=parse_data_expression(condition,free_variables,data_spec);
  enumerate(data_spec,
                              enum_vars,
                              cond,
                              number_of_solutions);
}

void empty_test()
{

  std::clog << "empty_test\n";
  typedef classic_enumerator< data::rewriter > enumerator_type;

  // test manual construction of evaluator with rewriter
  data::data_specification specification;
  data::rewriter           evaluator(specification);

  std::set< variable > variables;

  size_t count = 0;

  // explicit with condition evaluator and condition
  enumerator_type enumerator(specification,evaluator);

  for (enumerator_type::iterator i=enumerator.begin(variables,sort_bool::true_()); i != enumerator.end(); ++i)
  {
    count++;
  }
  BOOST_CHECK(count == 1);

  // explicit with condition but without condition evaluator
  for (enumerator_type::iterator i=enumerator.begin(variables,sort_bool::true_()); i != enumerator.end(); ++i)
  {
    count++;
  }
  BOOST_CHECK(count == 2);

  variables.insert(variable("y", sort_nat::nat()));

  for (enumerator_type::iterator i=enumerator.begin(variables, sort_bool::false_()); i != enumerator.end(); ++i)
  {
    BOOST_CHECK(false);
  }
}


void list_test()
{

  const std::string boolean_list_specification =
    "sort list_of_booleans;                                    \n"
    "cons empty : list_of_booleans;                            \n"
    "     lcons : Bool # list_of_booleans -> list_of_booleans; \n"
    "map  size   : list_of_booleans -> Nat;                    \n"
    "var  l : list_of_booleans;                                \n"
    "     b : Bool;                                            \n"
    "eqn  size(empty) = 0;                                     \n"
    "     size(lcons(b,l)) = 1 + size(l);                      \n"
    ;

  std::set< variable > variables;

  variables.insert(variable("x", basic_sort("list_of_booleans")));
  variables.insert(variable("y", basic_sort("Nat")));

  std::clog << "list_test\n";
  enumerate(parse_data_specification(boolean_list_specification),
                              variables, parse_data_expression("y == size(x) && 0 < y && y < 2",
                                  "x : list_of_booleans; y : Nat;", boolean_list_specification), 0);
}

void equality_substitution_test()
{
  const data_specification spec=parse_data_specification("sort L=Nat;");
  std::set< variable > variables;
  variables.insert(variable("x", basic_sort("Pos")));
  std::clog << "Test1 equality\n";
  enumerate(spec,
            variables,
            parse_data_expression("x==17 && x!=17", "x : Pos;", spec),
            0);
  std::clog << "Test2 equality\n";
  enumerate(spec,
            variables,
            parse_data_expression("x==17 && x==x", "x : Pos;", spec),
            1);
  std::clog << "Test3 equality\n";
  enumerate(spec,
            variables,
            parse_data_expression("x==17 && 2*x==34", "x : Pos;", spec),
            1);
  std::clog << "Test4 equality\n";
  variables.insert(variable("b", basic_sort("Bool")));
  enumerate(spec,
            variables,
            parse_data_expression("x==17 && 2*x==34", "x : Pos;", spec),
            2);
  std::clog << "Test4 equality: return two non exact solutions\n";
  std::set< variable > bvar;
  enumerate(spec,
            bvar, // intentionally empty.
            parse_data_expression("x==17 && 2*x==34", "x : Pos;", spec),
            1);
  bvar.insert(variable("b", basic_sort("Bool")));
  enumerate(spec,
            bvar,
            parse_data_expression("x==17 && 2*x==34", "x : Pos;", spec),
            2);
}

void tree_test()
{
  const std::string tree_specification =
    "sort tree_with_booleans;                                                   \n"
    "cons leaf : Bool -> tree_with_booleans;                                    \n"
    "cons node : tree_with_booleans # tree_with_booleans -> tree_with_booleans; \n"
    ;

  enumerate(tree_specification, "x : tree_with_booleans;", "true", "",32,true);
}

void mutually_recursive_test()
{
  const std::string mutually_recursive_sort_specification =
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

  std::clog << "tree_test1\n";
  enumerate(mutually_recursive_sort_specification, "x : this;", "true", "", 512,true);
  std::clog << "tree_test2\n";
  enumerate(mutually_recursive_sort_specification, "x : that;", "true", "", 512,true);
}

void check_concepts()
{
  using namespace mcrl2::data::concepts;

  BOOST_CONCEPT_ASSERT((Evaluator< mcrl2::data::rewriter, mutable_map_substitution< > >));
  BOOST_CONCEPT_ASSERT((classic_enumerator< >::iterator ));
}

inline
data::data_expression_vector generate_values(const data::data_specification& dataspec, const data::sort_expression& s, std::size_t max_size = 1000)
{
  std::size_t max_internal_variables = 10000;
  data::data_expression_vector result;

  data::rewriter rewr(dataspec);
  data::classic_enumerator<data::rewriter> enumerator(dataspec, rewr);
  data::variable x("x", s);
  data::variable_vector v;
  v.push_back(x);
  for (data::classic_enumerator<data::rewriter>::iterator i = enumerator.begin(v, data::sort_bool::true_(), max_internal_variables); i != enumerator.end() ; ++i)
  {
    result.push_back((*i)(x));
    if (result.size() >= max_size)
    {
      break;
    }
  }
  return result;
}

bool no_duplicates(const data::data_expression_vector& v)
{
  std::set<data::data_expression> s;
  s.insert(v.begin(), v.end());
  return s.size() == v.size();
}

void generate_values_test()
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
  data::data_specification dataspec = data::parse_data_specification(DATASPEC);

  const data::sort_expression_vector& sorts = dataspec.user_defined_sorts();
  for (data::sort_expression_vector::const_iterator i = sorts.begin(); i != sorts.end(); ++i)
  {
    data::sort_expression s = normalize_sorts(*i,dataspec);
    std::clog << "--- sort " << data::pp(s) << std::endl;
    data::data_expression_vector v = generate_values(dataspec, s, 10);
    std::clog << " possible values: " << core::detail::print_set(v, data::stream_printer()) << std::endl;
    BOOST_CHECK(v.size() <= 10);
    BOOST_CHECK(no_duplicates(v));
  }

  const data::alias_vector& aliases = dataspec.user_defined_aliases();
  for (data::alias_vector::const_iterator i = aliases.begin(); i != aliases.end(); ++i)
  {
    data::alias a = *i;
    data::sort_expression s = normalize_sorts(i->reference(),dataspec);
    std::clog << "--- sort " << data::pp(s) << std::endl;
    data::data_expression_vector v = generate_values(dataspec, s, 10);
    std::clog << " possible values: " << core::detail::print_set(v, data::stream_printer()) << std::endl;
    BOOST_CHECK(v.size() <= 10);
    BOOST_CHECK(no_duplicates(v));
  }
}

int test_main(int argc, char** argv)
{
  check_concepts();

  empty_test();

  list_test();
  tree_test();

  // This test does not seem to terminate after changing the values of the default constructor
  // for some terms. This looks like a bug in the enumerator (Wieger).
  mutually_recursive_test();

  equality_substitution_test();

  generate_values_test();

  return EXIT_SUCCESS;
}
