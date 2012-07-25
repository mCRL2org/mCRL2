// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/builder.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBES_H
#include "mcrl2/pbes/pbes.h"
#endif

#ifndef MCRL2_PBES_BUILDER_H
#define MCRL2_PBES_BUILDER_H

#include "mcrl2/core/builder.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief Builder class
template <typename Derived>
struct pbes_expression_builder_base: public core::builder<Derived>
{
  typedef core::builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  	
  data::data_expression operator()(const data::data_expression& x)
  {
  	return x;
  }
};

// Adds sort expression traversal to a builder
//--- start generated add_sort_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_sort_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  pbes_system::propositional_variable operator()(const pbes_system::propositional_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::propositional_variable result = pbes_system::propositional_variable(x.name(), static_cast<Derived&>(*this)(x.parameters()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  void operator()(pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    x.variable() = static_cast<Derived&>(*this)(x.variable());
    x.formula() = static_cast<Derived&>(*this)(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  template <typename Container>
  void operator()(pbes_system::pbes<Container>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    static_cast<Derived&>(*this)(x.global_variables());
    x.initial_state() = static_cast<Derived&>(*this)(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  pbes_system::pbes_expression operator()(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::propositional_variable_instantiation(x.name(), static_cast<Derived&>(*this)(x.parameters()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  pbes_system::pbes_expression operator()(const pbes_system::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  pbes_system::pbes_expression operator()(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::not_(static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::and_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::or_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::imp(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::forall(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::exists(static_cast<Derived&>(*this)(x.variables()), static_cast<Derived&>(*this)(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result;
    if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::propositional_variable_instantiation(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_true(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::true_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_false(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::false_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_not(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::not_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_and(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::and_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_or(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::or_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_imp(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::imp(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_forall(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::forall(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_exists(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::exists(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct sort_expression_builder: public add_sort_expressions<data::sort_expression_builder, Derived>
{
  typedef add_sort_expressions<data::sort_expression_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
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
  using super::operator();

  void operator()(pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    x.formula() = static_cast<Derived&>(*this)(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  template <typename Container>
  void operator()(pbes_system::pbes<Container>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    x.initial_state() = static_cast<Derived&>(*this)(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  pbes_system::pbes_expression operator()(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::propositional_variable_instantiation(x.name(), static_cast<Derived&>(*this)(x.parameters()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  pbes_system::pbes_expression operator()(const pbes_system::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  pbes_system::pbes_expression operator()(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::not_(static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::and_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::or_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::imp(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::forall(x.variables(), static_cast<Derived&>(*this)(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::exists(x.variables(), static_cast<Derived&>(*this)(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result;
    if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::propositional_variable_instantiation(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_true(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::true_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_false(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::false_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_not(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::not_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_and(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::and_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_or(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::or_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_imp(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::imp(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_forall(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::forall(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_exists(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::exists(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct data_expression_builder: public add_data_expressions<data::data_expression_builder, Derived>
{
  typedef add_data_expressions<data::data_expression_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_data_expressions code ---//

// Adds pbes expression traversal to a builder
//--- start generated add_pbes_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_pbes_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    x.formula() = static_cast<Derived&>(*this)(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  template <typename Container>
  void operator()(pbes_system::pbes<Container>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    static_cast<Derived&>(*this).leave(x);
  }

  pbes_system::pbes_expression operator()(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  pbes_system::pbes_expression operator()(const pbes_system::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  pbes_system::pbes_expression operator()(const pbes_system::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  pbes_system::pbes_expression operator()(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::not_(static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::and_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::or_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::imp(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::forall(x.variables(), static_cast<Derived&>(*this)(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result = pbes_system::exists(x.variables(), static_cast<Derived&>(*this)(x.body()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  pbes_system::pbes_expression operator()(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result;
    if (data::is_data_expression(x))
    {
      result = static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::propositional_variable_instantiation(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_true(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::true_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_false(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::false_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_not(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::not_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_and(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::and_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_or(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::or_(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_imp(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::imp(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_forall(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::forall(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    else if (pbes_system::is_exists(x))
    {
      result = static_cast<Derived&>(*this)(pbes_system::exists(atermpp::aterm_cast<atermpp::aterm_appl>(x)));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct pbes_expression_builder: public add_pbes_expressions<pbes_system::pbes_expression_builder_base, Derived>
{
  typedef add_pbes_expressions<pbes_system::pbes_expression_builder_base, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_pbes_expressions code ---//

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_BUILDER_H
