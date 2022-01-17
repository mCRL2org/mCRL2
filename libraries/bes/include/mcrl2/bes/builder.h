// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
  using super::update;
  using super::apply;

  void update(bes::boolean_equation& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    boolean_expression result_formula;
    static_cast<Derived&>(*this).apply(result_formula, x.formula());
    x.formula() = result_formula;
    static_cast<Derived&>(*this).leave(x);
  }

  void update(bes::boolean_equation_system& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).update(x.equations());
    boolean_expression result_initial_state;
    static_cast<Derived&>(*this).apply(result_initial_state, x.initial_state());
    x.initial_state() = result_initial_state;
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const bes::true_& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    result = x;
  }

  template <class T>
  void apply(T& result, const bes::false_& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    result = x;
  }

  template <class T>
  void apply(T& result, const bes::not_& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    bes::make_not_(result, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.operand()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const bes::and_& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    bes::make_and_(result, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const bes::or_& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    bes::make_or_(result, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const bes::imp& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    bes::make_imp(result, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const bes::boolean_variable& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    result = x;
  }

  template <class T>
  void apply(T& result, const bes::boolean_expression& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    if (bes::is_true(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::true_>(x));
    }
    else if (bes::is_false(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::false_>(x));
    }
    else if (bes::is_not(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::not_>(x));
    }
    else if (bes::is_and(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::and_>(x));
    }
    else if (bes::is_or(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::or_>(x));
    }
    else if (bes::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::imp>(x));
    }
    else if (bes::is_boolean_variable(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::boolean_variable>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Builder class
template <typename Derived>
struct boolean_expression_builder: public add_boolean_expressions<core::builder, Derived>
{
};
//--- end generated add_boolean_expressions code ---//

//--- start generated add_boolean_variables code ---//
template <template <class> class Builder, class Derived>
struct add_boolean_variables: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  void update(bes::boolean_equation& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    boolean_variable result_variable;
    static_cast<Derived&>(*this).apply(result_variable, x.variable());
    x.variable() = result_variable;
    boolean_expression result_formula;
    static_cast<Derived&>(*this).apply(result_formula, x.formula());
    x.formula() = result_formula;
    static_cast<Derived&>(*this).leave(x);
  }

  void update(bes::boolean_equation_system& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).update(x.equations());
    boolean_expression result_initial_state;
    static_cast<Derived&>(*this).apply(result_initial_state, x.initial_state());
    x.initial_state() = result_initial_state;
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const bes::true_& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    result = x;
  }

  template <class T>
  void apply(T& result, const bes::false_& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    result = x;
  }

  template <class T>
  void apply(T& result, const bes::not_& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    bes::make_not_(result, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.operand()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const bes::and_& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    bes::make_and_(result, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const bes::or_& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    bes::make_or_(result, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const bes::imp& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    bes::make_imp(result, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.left()); }, [&](boolean_expression& result){ static_cast<Derived&>(*this).apply(result, x.right()); });
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const bes::boolean_variable& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    result = x;
  }

  template <class T>
  void apply(T& result, const bes::boolean_expression& x)
  { 
    static_cast<Derived&>(*this).enter(x);
    if (bes::is_true(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::true_>(x));
    }
    else if (bes::is_false(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::false_>(x));
    }
    else if (bes::is_not(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::not_>(x));
    }
    else if (bes::is_and(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::and_>(x));
    }
    else if (bes::is_or(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::or_>(x));
    }
    else if (bes::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::imp>(x));
    }
    else if (bes::is_boolean_variable(x))
    {
      static_cast<Derived&>(*this).apply(result, atermpp::down_cast<bes::boolean_variable>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Builder class
template <typename Derived>
struct boolean_variable_builder: public add_boolean_variables<core::builder, Derived>
{
};
//--- end generated add_boolean_variables code ---//

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_BUILDER_H
