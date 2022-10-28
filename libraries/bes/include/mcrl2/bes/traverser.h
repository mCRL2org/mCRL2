// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/traverser.h
/// \brief add your file description here.

// To avoid circular inclusion problems
#ifndef MCRL2_BES_BOOLEAN_EQUATION_SYSTEM_H
#include "mcrl2/bes/boolean_equation_system.h"
#endif

#ifndef MCRL2_BES_TRAVERSER_H
#define MCRL2_BES_TRAVERSER_H

namespace mcrl2
{

namespace bes
{

//--- start generated add_traverser_boolean_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_boolean_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const bes::boolean_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::boolean_equation_system& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::boolean_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::boolean_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (bes::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::true_>(x));
    }
    else if (bes::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::false_>(x));
    }
    else if (bes::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::not_>(x));
    }
    else if (bes::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::and_>(x));
    }
    else if (bes::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::or_>(x));
    }
    else if (bes::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::imp>(x));
    }
    else if (bes::is_boolean_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::boolean_variable>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct boolean_expression_traverser: public add_traverser_boolean_expressions<core::traverser, Derived>
{
};
//--- end generated add_traverser_boolean_expressions code ---//

//--- start generated add_traverser_boolean_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_boolean_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const bes::boolean_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variable());
    static_cast<Derived&>(*this).apply(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::boolean_equation_system& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.equations());
    static_cast<Derived&>(*this).apply(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.left());
    static_cast<Derived&>(*this).apply(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::boolean_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const bes::boolean_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (bes::is_true(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::true_>(x));
    }
    else if (bes::is_false(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::false_>(x));
    }
    else if (bes::is_not(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::not_>(x));
    }
    else if (bes::is_and(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::and_>(x));
    }
    else if (bes::is_or(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::or_>(x));
    }
    else if (bes::is_imp(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::imp>(x));
    }
    else if (bes::is_boolean_variable(x))
    {
      static_cast<Derived&>(*this).apply(atermpp::down_cast<bes::boolean_variable>(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \\brief Traverser class
template <typename Derived>
struct boolean_variable_traverser: public add_traverser_boolean_variables<core::traverser, Derived>
{
};
//--- end generated add_traverser_boolean_variables code ---//

//--- start generated add_traverser_identifier_strings code ---//
//--- end generated add_traverser_identifier_strings code ---//

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_TRAVERSER_H
