// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/traverser.h
/// \brief add your file description here.

// To avoid circular header dependency problems
#ifndef MCRL2_DATA_DATA_SPECIFICATION_H
#include "mcrl2/data/data_specification.h"
#endif

#ifndef MCRL2_DATA_TRAVERSER_H
#define MCRL2_DATA_TRAVERSER_H

#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_base_of.hpp"

#include "mcrl2/core/traverser.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/multiple_possible_sorts.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/forall.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/unknown_sort.h"

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
  using super::operator();

  void operator()(const data::identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::function_symbol& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::application& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.head());
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::where_clause& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this)(x.declarations());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.lhs());
    static_cast<Derived&>(*this)(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::identifier_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::basic_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::container_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.element_sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::structured_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.constructors());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::function_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.domain());
    static_cast<Derived&>(*this)(x.codomain());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::unknown_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::multiple_possible_sorts& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.sorts());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::lambda& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::structured_sort_constructor_argument& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::structured_sort_constructor& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::data_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.lhs());
    static_cast<Derived&>(*this)(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::data_expression result;
    if (data::is_abstraction(x))
    {
      static_cast<Derived&>(*this)(data::abstraction(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_identifier(x))
    {
      static_cast<Derived&>(*this)(data::identifier(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this)(data::variable(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_function_symbol(x))
    {
      static_cast<Derived&>(*this)(data::function_symbol(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_application(x))
    {
      static_cast<Derived&>(*this)(data::application(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_where_clause(x))
    {
      static_cast<Derived&>(*this)(data::where_clause(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::assignment_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::assignment_expression result;
    if (data::is_assignment(x))
    {
      static_cast<Derived&>(*this)(data::assignment(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_identifier_assignment(x))
    {
      static_cast<Derived&>(*this)(data::identifier_assignment(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::sort_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::sort_expression result;
    if (data::is_basic_sort(x))
    {
      static_cast<Derived&>(*this)(data::basic_sort(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_container_sort(x))
    {
      static_cast<Derived&>(*this)(data::container_sort(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_structured_sort(x))
    {
      static_cast<Derived&>(*this)(data::structured_sort(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_function_sort(x))
    {
      static_cast<Derived&>(*this)(data::function_sort(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_unknown_sort(x))
    {
      static_cast<Derived&>(*this)(data::unknown_sort(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_multiple_possible_sorts(x))
    {
      static_cast<Derived&>(*this)(data::multiple_possible_sorts(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::abstraction& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::abstraction result;
    if (data::is_forall(x))
    {
      static_cast<Derived&>(*this)(data::forall(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_exists(x))
    {
      static_cast<Derived&>(*this)(data::exists(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_lambda(x))
    {
      static_cast<Derived&>(*this)(data::lambda(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct sort_expression_traverser: public add_traverser_sort_expressions<core::traverser, Derived>
{
  typedef add_traverser_sort_expressions<core::traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_traverser_sort_expressions code ---//

//--- start generated add_traverser_data_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_data_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const data::identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::function_symbol& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::application& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.head());
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::where_clause& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this)(x.declarations());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::identifier_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::lambda& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::data_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.lhs());
    static_cast<Derived&>(*this)(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::data_expression result;
    if (data::is_abstraction(x))
    {
      static_cast<Derived&>(*this)(data::abstraction(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_identifier(x))
    {
      static_cast<Derived&>(*this)(data::identifier(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this)(data::variable(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_function_symbol(x))
    {
      static_cast<Derived&>(*this)(data::function_symbol(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_application(x))
    {
      static_cast<Derived&>(*this)(data::application(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_where_clause(x))
    {
      static_cast<Derived&>(*this)(data::where_clause(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::assignment_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::assignment_expression result;
    if (data::is_assignment(x))
    {
      static_cast<Derived&>(*this)(data::assignment(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_identifier_assignment(x))
    {
      static_cast<Derived&>(*this)(data::identifier_assignment(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::abstraction& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::abstraction result;
    if (data::is_forall(x))
    {
      static_cast<Derived&>(*this)(data::forall(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_exists(x))
    {
      static_cast<Derived&>(*this)(data::exists(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_lambda(x))
    {
      static_cast<Derived&>(*this)(data::lambda(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct data_expression_traverser: public add_traverser_data_expressions<core::traverser, Derived>
{
  typedef add_traverser_data_expressions<core::traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_traverser_data_expressions code ---//

//--- start generated add_traverser_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const data::identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::function_symbol& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::application& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.head());
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::where_clause& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this)(x.declarations());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.lhs());
    static_cast<Derived&>(*this)(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::identifier_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::lambda& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::data_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.lhs());
    static_cast<Derived&>(*this)(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::data_expression result;
    if (data::is_abstraction(x))
    {
      static_cast<Derived&>(*this)(data::abstraction(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_identifier(x))
    {
      static_cast<Derived&>(*this)(data::identifier(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this)(data::variable(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_function_symbol(x))
    {
      static_cast<Derived&>(*this)(data::function_symbol(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_application(x))
    {
      static_cast<Derived&>(*this)(data::application(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_where_clause(x))
    {
      static_cast<Derived&>(*this)(data::where_clause(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::assignment_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::assignment_expression result;
    if (data::is_assignment(x))
    {
      static_cast<Derived&>(*this)(data::assignment(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_identifier_assignment(x))
    {
      static_cast<Derived&>(*this)(data::identifier_assignment(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::abstraction& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::abstraction result;
    if (data::is_forall(x))
    {
      static_cast<Derived&>(*this)(data::forall(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_exists(x))
    {
      static_cast<Derived&>(*this)(data::exists(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_lambda(x))
    {
      static_cast<Derived&>(*this)(data::lambda(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct variable_traverser: public add_traverser_variables<core::traverser, Derived>
{
  typedef add_traverser_variables<core::traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_traverser_variables code ---//

//--- start generated add_traverser_identifier_strings code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_identifier_strings: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const data::identifier& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.name());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.name());
    static_cast<Derived&>(*this)(x.sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::function_symbol& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.name());
    static_cast<Derived&>(*this)(x.sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::application& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.head());
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::where_clause& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this)(x.declarations());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.lhs());
    static_cast<Derived&>(*this)(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::identifier_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.lhs());
    static_cast<Derived&>(*this)(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::basic_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.name());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::container_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.element_sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::structured_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.constructors());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::function_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.domain());
    static_cast<Derived&>(*this)(x.codomain());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::unknown_sort& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::multiple_possible_sorts& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.sorts());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::lambda& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::structured_sort_constructor_argument& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.name());
    static_cast<Derived&>(*this)(x.sort());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::structured_sort_constructor& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.name());
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this)(x.recogniser());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::data_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.lhs());
    static_cast<Derived&>(*this)(x.rhs());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::data_expression result;
    if (data::is_abstraction(x))
    {
      static_cast<Derived&>(*this)(data::abstraction(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_identifier(x))
    {
      static_cast<Derived&>(*this)(data::identifier(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_variable(x))
    {
      static_cast<Derived&>(*this)(data::variable(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_function_symbol(x))
    {
      static_cast<Derived&>(*this)(data::function_symbol(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_application(x))
    {
      static_cast<Derived&>(*this)(data::application(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_where_clause(x))
    {
      static_cast<Derived&>(*this)(data::where_clause(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::assignment_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::assignment_expression result;
    if (data::is_assignment(x))
    {
      static_cast<Derived&>(*this)(data::assignment(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_identifier_assignment(x))
    {
      static_cast<Derived&>(*this)(data::identifier_assignment(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::sort_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::sort_expression result;
    if (data::is_basic_sort(x))
    {
      static_cast<Derived&>(*this)(data::basic_sort(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_container_sort(x))
    {
      static_cast<Derived&>(*this)(data::container_sort(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_structured_sort(x))
    {
      static_cast<Derived&>(*this)(data::structured_sort(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_function_sort(x))
    {
      static_cast<Derived&>(*this)(data::function_sort(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_unknown_sort(x))
    {
      static_cast<Derived&>(*this)(data::unknown_sort(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_multiple_possible_sorts(x))
    {
      static_cast<Derived&>(*this)(data::multiple_possible_sorts(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const data::abstraction& x)
  {
    static_cast<Derived&>(*this).enter(x);
    data::abstraction result;
    if (data::is_forall(x))
    {
      static_cast<Derived&>(*this)(data::forall(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_exists(x))
    {
      static_cast<Derived&>(*this)(data::exists(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (data::is_lambda(x))
    {
      static_cast<Derived&>(*this)(data::lambda(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct identifier_string_traverser: public add_traverser_identifier_strings<core::traverser, Derived>
{
  typedef add_traverser_identifier_strings<core::traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_traverser_identifier_strings code ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_TRAVERSER_H
