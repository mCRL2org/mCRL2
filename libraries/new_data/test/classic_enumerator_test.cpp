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
#include "mcrl2/new_data/data_expression.h"
#include "mcrl2/new_data/parser.h"
#include "mcrl2/new_data/rewriter.h"
#include "mcrl2/new_data/expression_traits.h"
#include "mcrl2/new_data/classic_enumerator.h"
#include "mcrl2/new_data/substitution.h"
#include "mcrl2/new_data/detail/concepts.h"

using namespace mcrl2;
using namespace mcrl2::new_data;

template < typename T >
void enumerate(data_specification const& d,
         std::set< variable > const& v,
                 data_expression const& c, size_t t);

template < typename T >
void enumerate(data_specification const& d,
                variable const& v,
                 data_expression const& condition, size_t t = 1000) {

  std::set< variable > variables;

  variables.insert(v);

  enumerate< T >(d, variables, condition, t);
}

template < typename T >
void enumerate(data_specification const& d,
                variable const& v, size_t t = 1000) {

  enumerate< T >(d, v, v, t);
}

// specialisation for classic_enumerator
template < >
void enumerate< classic_enumerator< > >(data_specification const& d,
         std::set< variable > const& v,
                 data_expression const& c, size_t t) {

  for (classic_enumerator< > i(d, v, c); t != 0 && i != classic_enumerator< >(); --t, ++i) {
    std::cout << mcrl2::core::pp((*i)(c)) << std::endl;
  }
}

#include "mcrl2/new_data/enumerator.h"

// specialisation for new_data::enumerator
template <>
void enumerate< data_enumerator< > >(data_specification const& d,
                                            std::set< variable > const& v,
                                              data_expression const& c, size_t t) {
  struct stack {
    std::stack< data_expression_with_variables > m_stack;

    void push(atermpp::vector< data_expression_with_variables > const& expressions) {
      for (atermpp::vector< data_expression_with_variables >::const_iterator i = expressions.begin(); i != expressions.end(); ++i) {
        m_stack.push(*i);
      }
    }

    data_expression_with_variables pop() {
      data_expression_with_variables result(m_stack.top());

      m_stack.pop();

      return result;
    }

    bool empty() {
      return m_stack.empty();
    }
  };

  rewriter rewr(d);
  number_postfix_generator generator("x_");
  data_enumerator<number_postfix_generator> e(d, rewr, generator);

  stack expression_stack;

  expression_stack.push(e.enumerate(data_expression_with_variables(c)));

  while (!expression_stack.empty() && t-- != 0) {
    data_expression_with_variables expression(expression_stack.pop());

//    std::cout << mcrl2::core::pp(expression) << std::endl;

    if (!expression.is_constant()) {
      expression_stack.push(e.enumerate(expression));
    }
  }
}

#include "mcrl2/new_data/detail/enum/enumerator.h"

// specialisation for new_data::detail::Enumerator
template <>
void enumerate< detail::Enumerator >(data_specification const& d,
                                std::set< variable > const& v,
                                    data_expression const& c, size_t t) {

  new_data::rewriter                            rewriter(d);

  rewriter(c); // forces data implementation and that proper rewrite rules are added

  std::auto_ptr< new_data::detail::Enumerator > enumerator(detail::createEnumerator(
      detail::data_specification_to_aterm_data_spec(d), &rewriter.get_rewriter()));

  variable_list variables;

  for (std::set< variable >::const_iterator i = v.begin(); i != v.end(); ++i) {
    atermpp::push_front(variables, *i);
  }

  std::auto_ptr< detail::EnumeratorSolutions > solutions(enumerator->findSolutions(
      static_cast< ATermList >(variables), rewriter.get_rewriter().toRewriteFormat(c)));

  ATermList substitution;

  while(!solutions->errorOccurred() && solutions->next(&substitution) && t-- != 0) {
    rewriter.get_rewriter().setSubstitutionInternalList(substitution);

    std::cout << mcrl2::core::pp(rewriter.get_rewriter().rewrite(static_cast< ATermAppl >(c))) << std::endl;

    rewriter.get_rewriter().clearSubstitutions();
  }

  BOOST_CHECK(!solutions->errorOccurred());
}

void empty_test() {
  using namespace mcrl2::new_data::selectors;

  typedef classic_enumerator< mutable_substitution< >, new_data::rewriter, select_not< false > >  enumerator_type;

  // test manual construction of evaluator with rewriter
  new_data::rewriter evaluator;

  std::set< variable > variables;

  // explicit with condition evaluator and condition
  for (enumerator_type i(data_specification(), variables, evaluator); i != enumerator_type(); ++i) {
    BOOST_CHECK(i->begin() == i->end()); // trivial valuation
  }

  // explicit with condition but without condition evaluator
  for (enumerator_type i(data_specification(), variables); i != enumerator_type(); ++i) {
    BOOST_CHECK(i->begin() == i->end()); //trivial valuation
  }

  variables.insert(variable("y", sort_nat::nat()));

  for (enumerator_type i(data_specification(), variables, sort_bool_::false_()); i != enumerator_type(); ++i) {
    BOOST_CHECK(false);
  }
}

template < typename EnumeratorType >
void enumerate_upto_a_maximum(std::string const& specification, std::string const& variable, const size_t count) {
  data_specification data_specification(parse_data_specification(specification));

  enumerate< EnumeratorType >(data_specification,
    parse_variable(variable, specification), count);
};

template < typename EnumeratorType >
void list_test(const size_t count) {
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

  enumerate< EnumeratorType >(parse_data_specification(boolean_list_specification),
        variables, parse_data_expression("y == size(x) && 0 < y && y < 2",
                         "x : list_of_booleans; y : Nat;", boolean_list_specification), count);
}

template < typename EnumeratorType >
void tree_test(const size_t count) {
  const std::string tree_specification =
    "sort tree_with_booleans;                                                   \n"
    "cons leaf : Bool -> tree_with_booleans;                                    \n"
    "cons node : tree_with_booleans # tree_with_booleans -> tree_with_booleans; \n"
  ;

  enumerate_upto_a_maximum< EnumeratorType >(tree_specification, "x : tree_with_booleans", count);
}

template < typename EnumeratorType >
void mutually_recursive_test(const size_t count) {
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

  enumerate_upto_a_maximum< EnumeratorType >(mutually_recursive_sort_specification, "x : this", count);
  enumerate_upto_a_maximum< EnumeratorType >(mutually_recursive_sort_specification, "x : that", count);
}

void check_concepts() {
  using namespace mcrl2::new_data::concepts;

  BOOST_CONCEPT_ASSERT((Evaluator< rewriter, mutable_substitution< > >));

  BOOST_CONCEPT_ASSERT((Enumerator< classic_enumerator< > >));
}

int test_main(int argc, char** argv) {
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv);

  check_concepts();

  empty_test();

  list_test< classic_enumerator< > >(512);
  tree_test< classic_enumerator< > >(1096);
  mutually_recursive_test< classic_enumerator< > >(1096);

  return EXIT_SUCCESS;
}
