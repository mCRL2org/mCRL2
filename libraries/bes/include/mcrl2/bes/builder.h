// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/builder.h
/// \brief add your file description here.

#ifndef MCRL2_BES_BUILDER_H
#define MCRL2_BES_BUILDER_H

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/core/builder.h"

namespace mcrl2
{

namespace bes
{

//--- start generated add_boolean_expressions code ---//
template <template <class> class Builder, class Derived>
struct add_boolean_expressions: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(bes::boolean_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    x.formula() = static_cast<Derived&>(*this)(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(bes::boolean_equation_system& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    x.initial_state() = static_cast<Derived&>(*this)(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  bes::boolean_expression operator()(const bes::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  bes::boolean_expression operator()(const bes::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  bes::boolean_expression operator()(const bes::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    bes::boolean_expression result = bes::not_(static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  bes::boolean_expression operator()(const bes::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    bes::boolean_expression result = bes::and_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  bes::boolean_expression operator()(const bes::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    bes::boolean_expression result = bes::or_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  bes::boolean_expression operator()(const bes::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    bes::boolean_expression result = bes::imp(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  bes::boolean_expression operator()(const bes::boolean_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  bes::boolean_expression operator()(const bes::boolean_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    bes::boolean_expression result;
    if (bes::is_true(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::true_>(x));
    }
    else if (bes::is_false(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::false_>(x));
    }
    else if (bes::is_not(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::not_>(x));
    }
    else if (bes::is_and(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::and_>(x));
    }
    else if (bes::is_or(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::or_>(x));
    }
    else if (bes::is_imp(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::imp>(x));
    }
    else if (bes::is_boolean_variable(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::boolean_variable>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct boolean_expression_builder: public add_boolean_expressions<core::builder, Derived>
{
  typedef add_boolean_expressions<core::builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_boolean_expressions code ---//

//--- start generated add_boolean_variables code ---//
template <template <class> class Builder, class Derived>
struct add_boolean_variables: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(bes::boolean_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    x.variable() = core::down_cast<bes::boolean_variable>(static_cast<Derived&>(*this)(x.variable()));
    x.formula() = static_cast<Derived&>(*this)(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(bes::boolean_equation_system& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    x.initial_state() = static_cast<Derived&>(*this)(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  bes::boolean_expression operator()(const bes::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  bes::boolean_expression operator()(const bes::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  bes::boolean_expression operator()(const bes::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    bes::boolean_expression result = bes::not_(static_cast<Derived&>(*this)(x.operand()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  bes::boolean_expression operator()(const bes::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    bes::boolean_expression result = bes::and_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  bes::boolean_expression operator()(const bes::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    bes::boolean_expression result = bes::or_(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  bes::boolean_expression operator()(const bes::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    bes::boolean_expression result = bes::imp(static_cast<Derived&>(*this)(x.left()), static_cast<Derived&>(*this)(x.right()));
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

  bes::boolean_expression operator()(const bes::boolean_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }

  bes::boolean_expression operator()(const bes::boolean_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    bes::boolean_expression result;
    if (bes::is_true(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::true_>(x));
    }
    else if (bes::is_false(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::false_>(x));
    }
    else if (bes::is_not(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::not_>(x));
    }
    else if (bes::is_and(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::and_>(x));
    }
    else if (bes::is_or(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::or_>(x));
    }
    else if (bes::is_imp(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::imp>(x));
    }
    else if (bes::is_boolean_variable(x))
    {
      result = static_cast<Derived&>(*this)(atermpp::aterm_cast<bes::boolean_variable>(x));
    }
    static_cast<Derived&>(*this).leave(x);
    return result;
  }

};

/// \brief Builder class
template <typename Derived>
struct boolean_variable_builder: public add_boolean_variables<core::builder, Derived>
{
  typedef add_boolean_variables<core::builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_boolean_variables code ---//

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_BUILDER_H
