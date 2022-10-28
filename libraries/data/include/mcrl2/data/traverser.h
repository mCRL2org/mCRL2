// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_TRAVERSER_H
#define MCRL2_DATA_TRAVERSER_H

#include "mcrl2/core/traverser.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/data/bag_comprehension.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/forall.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/set_comprehension.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/untyped_data_parameter.h"
#include "mcrl2/data/untyped_possible_sorts.h"
#include "mcrl2/data/untyped_set_or_bag_comprehension.h"
#include "mcrl2/data/untyped_sort_variable.h"
#include "mcrl2/data/where_clause.h"

namespace mcrl2
{

namespace data
{

// Adds sort expression traversal to a traverser
//--- start generated add_traverser_sort_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_sort_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const data::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::function_symbol& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::application& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.head());
    for (const data_expression& t: x) { static_cast<Derived&>(*this).apply(t); }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::where_clause& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).apply(x.declarations());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.lhs());
    static_cast<Derived&>(*this).apply(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_identifier_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::basic_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::container_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.element_sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::structured_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.constructors());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::function_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.domain());
    static_cast<Derived&>(*this).apply(x.codomain());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_possible_sorts& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.sorts());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_sort_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::lambda& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::set_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_set_or_bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::structured_sort_constructor_argument& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::structured_sort_constructor& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::alias& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.reference());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::data_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.lhs());
    static_cast<Derived&>(*this).apply(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_data_parameter& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_abstraction(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::abstraction>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_function_symbol(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::function_symbol>(x));
    }
    else if (data::is_application(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::application>(x));
    }
    else if (data::is_where_clause(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::where_clause>(x));
    }
    else if (data::is_untyped_identifier(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::assignment_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::assignment>(x));
    }
    else if (data::is_untyped_identifier_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier_assignment>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::sort_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_basic_sort(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::basic_sort>(x));
    }
    else if (data::is_container_sort(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::container_sort>(x));
    }
    else if (data::is_structured_sort(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::structured_sort>(x));
    }
    else if (data::is_function_sort(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::function_sort>(x));
    }
    else if (data::is_untyped_sort(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_sort>(x));
    }
    else if (data::is_untyped_possible_sorts(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_possible_sorts>(x));
    }
    else if (data::is_untyped_sort_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_sort_variable>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::abstraction& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::forall>(x));
    }
    else if (data::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::exists>(x));
    }
    else if (data::is_lambda(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::lambda>(x));
    }
    else if (data::is_set_comprehension(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::set_comprehension>(x));
    }
    else if (data::is_bag_comprehension(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::bag_comprehension>(x));
    }
    else if (data::is_untyped_set_or_bag_comprehension(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_set_or_bag_comprehension>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct sort_expression_traverser: public add_traverser_sort_expressions<core::traverser, Derived>
{
};
//--- end generated add_traverser_sort_expressions code ---//

//--- start generated add_traverser_data_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_data_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const data::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::function_symbol& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::application& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.head());
    for (const data_expression& t: x) { static_cast<Derived&>(*this).apply(t); }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::where_clause& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).apply(x.declarations());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_identifier_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::lambda& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::set_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_set_or_bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::data_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.lhs());
    static_cast<Derived&>(*this).apply(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_data_parameter& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_abstraction(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::abstraction>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_function_symbol(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::function_symbol>(x));
    }
    else if (data::is_application(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::application>(x));
    }
    else if (data::is_where_clause(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::where_clause>(x));
    }
    else if (data::is_untyped_identifier(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::assignment_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::assignment>(x));
    }
    else if (data::is_untyped_identifier_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier_assignment>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::abstraction& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::forall>(x));
    }
    else if (data::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::exists>(x));
    }
    else if (data::is_lambda(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::lambda>(x));
    }
    else if (data::is_set_comprehension(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::set_comprehension>(x));
    }
    else if (data::is_bag_comprehension(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::bag_comprehension>(x));
    }
    else if (data::is_untyped_set_or_bag_comprehension(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_set_or_bag_comprehension>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct data_expression_traverser: public add_traverser_data_expressions<core::traverser, Derived>
{
};
//--- end generated add_traverser_data_expressions code ---//

//--- start generated add_traverser_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const data::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::function_symbol& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::application& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.head());
    for (const data_expression& t: x) { static_cast<Derived&>(*this).apply(t); }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::where_clause& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).apply(x.declarations());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.lhs());
    static_cast<Derived&>(*this).apply(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_identifier_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::lambda& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::set_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_set_or_bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::data_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.lhs());
    static_cast<Derived&>(*this).apply(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_data_parameter& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_abstraction(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::abstraction>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_function_symbol(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::function_symbol>(x));
    }
    else if (data::is_application(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::application>(x));
    }
    else if (data::is_where_clause(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::where_clause>(x));
    }
    else if (data::is_untyped_identifier(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::assignment_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::assignment>(x));
    }
    else if (data::is_untyped_identifier_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier_assignment>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::abstraction& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::forall>(x));
    }
    else if (data::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::exists>(x));
    }
    else if (data::is_lambda(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::lambda>(x));
    }
    else if (data::is_set_comprehension(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::set_comprehension>(x));
    }
    else if (data::is_bag_comprehension(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::bag_comprehension>(x));
    }
    else if (data::is_untyped_set_or_bag_comprehension(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_set_or_bag_comprehension>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct variable_traverser: public add_traverser_variables<core::traverser, Derived>
{
};
//--- end generated add_traverser_variables code ---//

//--- start generated add_traverser_identifier_strings code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_identifier_strings: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const data::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::function_symbol& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::application& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.head());
    for (const data_expression& t: x) { static_cast<Derived&>(*this).apply(t); }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::where_clause& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).apply(x.declarations());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.lhs());
    static_cast<Derived&>(*this).apply(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_identifier_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.lhs());
    static_cast<Derived&>(*this).apply(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::basic_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::container_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.element_sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::structured_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.constructors());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::function_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.domain());
    static_cast<Derived&>(*this).apply(x.codomain());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_possible_sorts& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.sorts());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_sort_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::lambda& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::set_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_set_or_bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::structured_sort_constructor_argument& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::structured_sort_constructor& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).apply(x.recogniser());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::alias& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.reference());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::data_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.lhs());
    static_cast<Derived&>(*this).apply(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::untyped_data_parameter& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.name());
    static_cast<Derived&>(*this).apply(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_abstraction(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::abstraction>(x));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_function_symbol(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::function_symbol>(x));
    }
    else if (data::is_application(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::application>(x));
    }
    else if (data::is_where_clause(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::where_clause>(x));
    }
    else if (data::is_untyped_identifier(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::assignment_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::assignment>(x));
    }
    else if (data::is_untyped_identifier_assignment(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier_assignment>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::sort_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_basic_sort(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::basic_sort>(x));
    }
    else if (data::is_container_sort(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::container_sort>(x));
    }
    else if (data::is_structured_sort(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::structured_sort>(x));
    }
    else if (data::is_function_sort(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::function_sort>(x));
    }
    else if (data::is_untyped_sort(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_sort>(x));
    }
    else if (data::is_untyped_possible_sorts(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_possible_sorts>(x));
    }
    else if (data::is_untyped_sort_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_sort_variable>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const data::abstraction& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (data::is_forall(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::forall>(x));
    }
    else if (data::is_exists(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::exists>(x));
    }
    else if (data::is_lambda(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::lambda>(x));
    }
    else if (data::is_set_comprehension(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::set_comprehension>(x));
    }
    else if (data::is_bag_comprehension(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::bag_comprehension>(x));
    }
    else if (data::is_untyped_set_or_bag_comprehension(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_set_or_bag_comprehension>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct identifier_string_traverser: public add_traverser_identifier_strings<core::traverser, Derived>
{
};
//--- end generated add_traverser_identifier_strings code ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_TRAVERSER_H
