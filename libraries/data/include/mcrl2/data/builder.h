// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/builder.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_BUILDER_H
#define MCRL2_DATA_BUILDER_H

#include <boost/bind.hpp>
#include "mcrl2/core/builder.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/forall.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/set_comprehension.h"
#include "mcrl2/data/bag_comprehension.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/where_clause.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/untyped_possible_sorts.h"
#include "mcrl2/data/untyped_sort.h"
#include "mcrl2/data/untyped_set_or_bag_comprehension.h"
#include "mcrl2/data/untyped_identifier.h"


namespace mcrl2
{

namespace data
{

// Adds sort expression traversal to a builder
//--- start generated add_sort_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_sort_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  data::variable apply(const data::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::variable result = data::variable(x.name(), static_cast<Derived&>(*this).apply(x.sort()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::function_symbol apply(const data::function_symbol& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::function_symbol result = data::function_symbol(x.name(), static_cast<Derived&>(*this).apply(x.sort()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::application apply(const data::application& x)
  {
    static_cast<Derived&>(*this).enter(x);
    typedef data::data_expression (Derived::*function_pointer)(const data::data_expression&);
    function_pointer fp = &Derived::apply;
    data::application result = data::application(
       static_cast<Derived&>(*this).apply(x.head()),
       x.begin(),
       x.end(),
       boost::bind(fp, static_cast<Derived*>(this), _1)
    );
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::where_clause apply(const data::where_clause& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::where_clause result = data::where_clause(static_cast<Derived&>(*this).apply(x.body()), static_cast<Derived&>(*this).apply(x.declarations()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::untyped_identifier apply(const data::untyped_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  data::assignment apply(const data::assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::assignment result = data::assignment(static_cast<Derived&>(*this).apply(x.lhs()), static_cast<Derived&>(*this).apply(x.rhs()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::untyped_identifier_assignment apply(const data::untyped_identifier_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::untyped_identifier_assignment result = data::untyped_identifier_assignment(x.lhs(), static_cast<Derived&>(*this).apply(x.rhs()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::basic_sort apply(const data::basic_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  data::container_sort apply(const data::container_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::container_sort result = data::container_sort(x.container_name(), static_cast<Derived&>(*this).apply(x.element_sort()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::structured_sort apply(const data::structured_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::structured_sort result = data::structured_sort(static_cast<Derived&>(*this).apply(x.constructors()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::function_sort apply(const data::function_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::function_sort result = data::function_sort(static_cast<Derived&>(*this).apply(x.domain()), static_cast<Derived&>(*this).apply(x.codomain()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::untyped_sort apply(const data::untyped_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  data::untyped_possible_sorts apply(const data::untyped_possible_sorts& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::untyped_possible_sorts result = data::untyped_possible_sorts(static_cast<Derived&>(*this).apply(x.sorts()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::forall apply(const data::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::forall result = data::forall(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::exists apply(const data::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::exists result = data::exists(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::lambda apply(const data::lambda& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::lambda result = data::lambda(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::set_comprehension apply(const data::set_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::set_comprehension result = data::set_comprehension(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::bag_comprehension apply(const data::bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::bag_comprehension result = data::bag_comprehension(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::untyped_set_or_bag_comprehension apply(const data::untyped_set_or_bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::untyped_set_or_bag_comprehension result = data::untyped_set_or_bag_comprehension(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::structured_sort_constructor_argument apply(const data::structured_sort_constructor_argument& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::structured_sort_constructor_argument result = data::structured_sort_constructor_argument(x.name(), static_cast<Derived&>(*this).apply(x.sort()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::structured_sort_constructor apply(const data::structured_sort_constructor& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::structured_sort_constructor result = data::structured_sort_constructor(x.name(), static_cast<Derived&>(*this).apply(x.arguments()), x.recogniser());
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::alias apply(const data::alias& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::alias result = data::alias(x.name(), static_cast<Derived&>(*this).apply(x.reference()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::data_equation apply(const data::data_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::data_equation result = data::data_equation(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.condition()), static_cast<Derived&>(*this).apply(x.lhs()), static_cast<Derived&>(*this).apply(x.rhs()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::data_expression apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::data_expression result;
    if (data::is_abstraction(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::abstraction>(x));
    }
    else if (data::is_variable(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_function_symbol(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::function_symbol>(x));
    }
    else if (data::is_application(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::application>(x));
    }
    else if (data::is_where_clause(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::where_clause>(x));
    }
    else if (data::is_untyped_identifier(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::assignment_expression apply(const data::assignment_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::assignment_expression result;
    if (data::is_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::assignment>(x));
    }
    else if (data::is_untyped_identifier_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier_assignment>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::sort_expression apply(const data::sort_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::sort_expression result;
    if (data::is_basic_sort(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::basic_sort>(x));
    }
    else if (data::is_container_sort(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::container_sort>(x));
    }
    else if (data::is_structured_sort(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::structured_sort>(x));
    }
    else if (data::is_function_sort(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::function_sort>(x));
    }
    else if (data::is_untyped_sort(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_sort>(x));
    }
    else if (data::is_untyped_possible_sorts(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_possible_sorts>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::abstraction apply(const data::abstraction& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::abstraction result;
    if (data::is_forall(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::forall>(x));
    }
    else if (data::is_exists(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::exists>(x));
    }
    else if (data::is_lambda(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::lambda>(x));
    }
    else if (data::is_set_comprehension(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::set_comprehension>(x));
    }
    else if (data::is_bag_comprehension(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::bag_comprehension>(x));
    }
    else if (data::is_untyped_set_or_bag_comprehension(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_set_or_bag_comprehension>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct sort_expression_builder: public add_sort_expressions<core::builder, Derived>
{
  typedef add_sort_expressions<core::builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated add_sort_expressions code ---//

// Adds data expression traversal to a builder
//--- start generated add_data_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_data_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  data::variable apply(const data::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  data::function_symbol apply(const data::function_symbol& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  data::application apply(const data::application& x)
  {
    static_cast<Derived&>(*this).enter(x);
    typedef data::data_expression (Derived::*function_pointer)(const data::data_expression&);
    function_pointer fp = &Derived::apply;
    data::application result = data::application(
       static_cast<Derived&>(*this).apply(x.head()),
       x.begin(),
       x.end(),
       boost::bind(fp, static_cast<Derived*>(this), _1)
    );
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::where_clause apply(const data::where_clause& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::where_clause result = data::where_clause(static_cast<Derived&>(*this).apply(x.body()), static_cast<Derived&>(*this).apply(x.declarations()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::untyped_identifier apply(const data::untyped_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  data::assignment apply(const data::assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::assignment result = data::assignment(x.lhs(), static_cast<Derived&>(*this).apply(x.rhs()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::untyped_identifier_assignment apply(const data::untyped_identifier_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::untyped_identifier_assignment result = data::untyped_identifier_assignment(x.lhs(), static_cast<Derived&>(*this).apply(x.rhs()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::forall apply(const data::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::forall result = data::forall(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::exists apply(const data::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::exists result = data::exists(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::lambda apply(const data::lambda& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::lambda result = data::lambda(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::set_comprehension apply(const data::set_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::set_comprehension result = data::set_comprehension(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::bag_comprehension apply(const data::bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::bag_comprehension result = data::bag_comprehension(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::untyped_set_or_bag_comprehension apply(const data::untyped_set_or_bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::untyped_set_or_bag_comprehension result = data::untyped_set_or_bag_comprehension(x.variables(), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::data_equation apply(const data::data_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::data_equation result = data::data_equation(x.variables(), static_cast<Derived&>(*this).apply(x.condition()), static_cast<Derived&>(*this).apply(x.lhs()), static_cast<Derived&>(*this).apply(x.rhs()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::data_expression apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::data_expression result;
    if (data::is_abstraction(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::abstraction>(x));
    }
    else if (data::is_variable(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_function_symbol(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::function_symbol>(x));
    }
    else if (data::is_application(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::application>(x));
    }
    else if (data::is_where_clause(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::where_clause>(x));
    }
    else if (data::is_untyped_identifier(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::assignment_expression apply(const data::assignment_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::assignment_expression result;
    if (data::is_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::assignment>(x));
    }
    else if (data::is_untyped_identifier_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier_assignment>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::abstraction apply(const data::abstraction& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::abstraction result;
    if (data::is_forall(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::forall>(x));
    }
    else if (data::is_exists(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::exists>(x));
    }
    else if (data::is_lambda(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::lambda>(x));
    }
    else if (data::is_set_comprehension(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::set_comprehension>(x));
    }
    else if (data::is_bag_comprehension(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::bag_comprehension>(x));
    }
    else if (data::is_untyped_set_or_bag_comprehension(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_set_or_bag_comprehension>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct data_expression_builder: public add_data_expressions<core::builder, Derived>
{
  typedef add_data_expressions<core::builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated add_data_expressions code ---//

//--- start generated add_variables code ---//
template <template <class> class Builder, class Derived>
struct add_variables: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  data::variable apply(const data::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  data::function_symbol apply(const data::function_symbol& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  data::application apply(const data::application& x)
  {
    static_cast<Derived&>(*this).enter(x);
    typedef data::data_expression (Derived::*function_pointer)(const data::data_expression&);
    function_pointer fp = &Derived::apply;
    data::application result = data::application(
       static_cast<Derived&>(*this).apply(x.head()),
       x.begin(),
       x.end(),
       boost::bind(fp, static_cast<Derived*>(this), _1)
    );
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::where_clause apply(const data::where_clause& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::where_clause result = data::where_clause(static_cast<Derived&>(*this).apply(x.body()), static_cast<Derived&>(*this).apply(x.declarations()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::untyped_identifier apply(const data::untyped_identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  data::assignment apply(const data::assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::assignment result = data::assignment(static_cast<Derived&>(*this).apply(x.lhs()), static_cast<Derived&>(*this).apply(x.rhs()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::untyped_identifier_assignment apply(const data::untyped_identifier_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::untyped_identifier_assignment result = data::untyped_identifier_assignment(x.lhs(), static_cast<Derived&>(*this).apply(x.rhs()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::forall apply(const data::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::forall result = data::forall(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::exists apply(const data::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::exists result = data::exists(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::lambda apply(const data::lambda& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::lambda result = data::lambda(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::set_comprehension apply(const data::set_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::set_comprehension result = data::set_comprehension(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::bag_comprehension apply(const data::bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::bag_comprehension result = data::bag_comprehension(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::untyped_set_or_bag_comprehension apply(const data::untyped_set_or_bag_comprehension& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::untyped_set_or_bag_comprehension result = data::untyped_set_or_bag_comprehension(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::data_equation apply(const data::data_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::data_equation result = data::data_equation(static_cast<Derived&>(*this).apply(x.variables()), static_cast<Derived&>(*this).apply(x.condition()), static_cast<Derived&>(*this).apply(x.lhs()), static_cast<Derived&>(*this).apply(x.rhs()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::data_expression apply(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::data_expression result;
    if (data::is_abstraction(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::abstraction>(x));
    }
    else if (data::is_variable(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::variable>(x));
    }
    else if (data::is_function_symbol(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::function_symbol>(x));
    }
    else if (data::is_application(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::application>(x));
    }
    else if (data::is_where_clause(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::where_clause>(x));
    }
    else if (data::is_untyped_identifier(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::assignment_expression apply(const data::assignment_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::assignment_expression result;
    if (data::is_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::assignment>(x));
    }
    else if (data::is_untyped_identifier_assignment(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_identifier_assignment>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  data::abstraction apply(const data::abstraction& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::abstraction result;
    if (data::is_forall(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::forall>(x));
    }
    else if (data::is_exists(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::exists>(x));
    }
    else if (data::is_lambda(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::lambda>(x));
    }
    else if (data::is_set_comprehension(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::set_comprehension>(x));
    }
    else if (data::is_bag_comprehension(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::bag_comprehension>(x));
    }
    else if (data::is_untyped_set_or_bag_comprehension(x))
    {
      result = static_cast<Derived&>(*this).apply(atermpp::down_cast<data::untyped_set_or_bag_comprehension>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct variable_builder: public add_variables<core::builder, Derived>
{
  typedef add_variables<core::builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
};
//--- end generated add_variables code ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_BUILDER_H
