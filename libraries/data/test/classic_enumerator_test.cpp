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

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/detail/concepts.h"
#include "mcrl2/data/standard_utility.h"

using namespace mcrl2;
using namespace mcrl2::data;

void enumerate(const data_specification & d,
               const atermpp::set< variable > & v,
               const data_expression & c, 
               const size_t expected_no_of_solutions,
               const bool more_solutions_possible=false)
{

  typedef classic_enumerator< rewriter > enumerator_type;

  rewriter evaluator(d);
  enumerator_type enumerator(d,evaluator);
  size_t number_of_solutions=0;
  enumerator_type::iterator i;
  for (i=enumerator.begin(v, c); number_of_solutions< expected_no_of_solutions && i != enumerator.end(); ++i)
  {
    ATfprintf(stderr,"SOLUTION %d %t\n",number_of_solutions,(ATermAppl)evaluator(c,*i));
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
  atermpp::set < variable > enum_vars;
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

  atermpp::set< variable > variables;

  size_t count = 0;

  // explicit with condition evaluator and condition
  enumerator_type enumerator(specification,evaluator);
  for (enumerator_type::iterator i=enumerator.begin(variables,sort_bool::true_()); i != enumerator.end(); ++i, ++count)
  {
    BOOST_CHECK(i->begin() == i->end()); // trivial valuation
  }

  BOOST_CHECK(count == 1);

  // explicit with condition but without condition evaluator
  for (enumerator_type::iterator i=enumerator.begin(variables, sort_bool::true_()); i != enumerator.end(); ++i, ++count)
  {
    BOOST_CHECK(i->begin() == i->end()); //trivial valuation
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

  atermpp::set< variable > variables;

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
  atermpp::set< variable > variables;
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
  atermpp::set< variable > bvar;
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

  enumerate(tree_specification, "x : tree_with_booleans;", "true", "", 512,true);
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

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv);

  check_concepts();
  core::garbage_collect();

  empty_test();
  core::garbage_collect();

  list_test();
  core::garbage_collect();
  tree_test();
  core::garbage_collect();
  mutually_recursive_test();
  core::garbage_collect();
  equality_substitution_test();

  return EXIT_SUCCESS;
}
