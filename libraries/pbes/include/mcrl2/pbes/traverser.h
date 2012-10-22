// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/traverser.h
/// \brief add your file description here.

// To avoid circular inclusion problems
#ifndef MCRL2_PBES_H
#include "mcrl2/pbes/pbes.h"
#endif

#ifndef MCRL2_PBES_TRAVERSER_H
#define MCRL2_PBES_TRAVERSER_H

#include "mcrl2/data/traverser.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief Traversal class for pbes_expressions. Used as a base class for pbes_expression_traverser.
template <typename Derived>
struct pbes_expression_traverser_base: public core::traverser<Derived>
{
  typedef core::traverser<Derived> super;
  using super::operator();
  using super::enter;
  using super::leave;

  pbes_expression operator()(const data::data_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
    return x;
  }
};

//--- start generated add_traverser_sort_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_sort_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const pbes_system::propositional_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variable());
    static_cast<Derived&>(*this)(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  template <typename Container>
  void operator()(const pbes_system::pbes<Container>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    static_cast<Derived&>(*this)(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result;
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this)(pbes_system::propositional_variable_instantiation(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_true(x))
    {
      static_cast<Derived&>(*this)(pbes_system::true_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_false(x))
    {
      static_cast<Derived&>(*this)(pbes_system::false_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_not(x))
    {
      static_cast<Derived&>(*this)(pbes_system::not_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_and(x))
    {
      static_cast<Derived&>(*this)(pbes_system::and_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_or(x))
    {
      static_cast<Derived&>(*this)(pbes_system::or_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_imp(x))
    {
      static_cast<Derived&>(*this)(pbes_system::imp(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_forall(x))
    {
      static_cast<Derived&>(*this)(pbes_system::forall(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_exists(x))
    {
      static_cast<Derived&>(*this)(pbes_system::exists(atermpp::aterm_appl(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct sort_expression_traverser: public add_traverser_sort_expressions<data::sort_expression_traverser, Derived>
{
  typedef add_traverser_sort_expressions<data::sort_expression_traverser, Derived> super;
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

  void operator()(const pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  template <typename Container>
  void operator()(const pbes_system::pbes<Container>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    static_cast<Derived&>(*this)(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result;
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this)(pbes_system::propositional_variable_instantiation(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_true(x))
    {
      static_cast<Derived&>(*this)(pbes_system::true_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_false(x))
    {
      static_cast<Derived&>(*this)(pbes_system::false_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_not(x))
    {
      static_cast<Derived&>(*this)(pbes_system::not_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_and(x))
    {
      static_cast<Derived&>(*this)(pbes_system::and_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_or(x))
    {
      static_cast<Derived&>(*this)(pbes_system::or_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_imp(x))
    {
      static_cast<Derived&>(*this)(pbes_system::imp(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_forall(x))
    {
      static_cast<Derived&>(*this)(pbes_system::forall(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_exists(x))
    {
      static_cast<Derived&>(*this)(pbes_system::exists(atermpp::aterm_appl(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct data_expression_traverser: public add_traverser_data_expressions<data::data_expression_traverser, Derived>
{
  typedef add_traverser_data_expressions<data::data_expression_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_traverser_data_expressions code ---//

//--- start generated add_traverser_pbes_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_pbes_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  template <typename Container>
  void operator()(const pbes_system::pbes<Container>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result;
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this)(pbes_system::propositional_variable_instantiation(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_true(x))
    {
      static_cast<Derived&>(*this)(pbes_system::true_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_false(x))
    {
      static_cast<Derived&>(*this)(pbes_system::false_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_not(x))
    {
      static_cast<Derived&>(*this)(pbes_system::not_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_and(x))
    {
      static_cast<Derived&>(*this)(pbes_system::and_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_or(x))
    {
      static_cast<Derived&>(*this)(pbes_system::or_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_imp(x))
    {
      static_cast<Derived&>(*this)(pbes_system::imp(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_forall(x))
    {
      static_cast<Derived&>(*this)(pbes_system::forall(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_exists(x))
    {
      static_cast<Derived&>(*this)(pbes_system::exists(atermpp::aterm_appl(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct pbes_expression_traverser: public add_traverser_pbes_expressions<pbes_system::pbes_expression_traverser_base, Derived>
{
  typedef add_traverser_pbes_expressions<pbes_system::pbes_expression_traverser_base, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_traverser_pbes_expressions code ---//

//--- start generated add_traverser_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const pbes_system::propositional_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variable());
    static_cast<Derived&>(*this)(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  template <typename Container>
  void operator()(const pbes_system::pbes<Container>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    static_cast<Derived&>(*this)(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result;
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this)(pbes_system::propositional_variable_instantiation(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_true(x))
    {
      static_cast<Derived&>(*this)(pbes_system::true_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_false(x))
    {
      static_cast<Derived&>(*this)(pbes_system::false_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_not(x))
    {
      static_cast<Derived&>(*this)(pbes_system::not_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_and(x))
    {
      static_cast<Derived&>(*this)(pbes_system::and_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_or(x))
    {
      static_cast<Derived&>(*this)(pbes_system::or_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_imp(x))
    {
      static_cast<Derived&>(*this)(pbes_system::imp(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_forall(x))
    {
      static_cast<Derived&>(*this)(pbes_system::forall(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_exists(x))
    {
      static_cast<Derived&>(*this)(pbes_system::exists(atermpp::aterm_appl(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct variable_traverser: public add_traverser_variables<data::variable_traverser, Derived>
{
  typedef add_traverser_variables<data::variable_traverser, Derived> super;
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

  void operator()(const pbes_system::propositional_variable& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.name());
    static_cast<Derived&>(*this)(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::pbes_equation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variable());
    static_cast<Derived&>(*this)(x.formula());
    static_cast<Derived&>(*this).leave(x);
  }

  template <typename Container>
  void operator()(const pbes_system::pbes<Container>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    static_cast<Derived&>(*this)(x.initial_state());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::propositional_variable_instantiation& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.name());
    static_cast<Derived&>(*this)(x.parameters());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::true_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::false_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::not_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.operand());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::and_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::or_& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::imp& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.left());
    static_cast<Derived&>(*this)(x.right());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::forall& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::exists& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.variables());
    static_cast<Derived&>(*this)(x.body());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const pbes_system::pbes_expression& x)
  {
    static_cast<Derived&>(*this).enter(x);
    pbes_system::pbes_expression result;
    if (data::is_data_expression(x))
    {
      static_cast<Derived&>(*this)(data::data_expression(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_propositional_variable_instantiation(x))
    {
      static_cast<Derived&>(*this)(pbes_system::propositional_variable_instantiation(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_true(x))
    {
      static_cast<Derived&>(*this)(pbes_system::true_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_false(x))
    {
      static_cast<Derived&>(*this)(pbes_system::false_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_not(x))
    {
      static_cast<Derived&>(*this)(pbes_system::not_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_and(x))
    {
      static_cast<Derived&>(*this)(pbes_system::and_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_or(x))
    {
      static_cast<Derived&>(*this)(pbes_system::or_(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_imp(x))
    {
      static_cast<Derived&>(*this)(pbes_system::imp(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_forall(x))
    {
      static_cast<Derived&>(*this)(pbes_system::forall(atermpp::aterm_appl(x)));
    }
    else if (pbes_system::is_exists(x))
    {
      static_cast<Derived&>(*this)(pbes_system::exists(atermpp::aterm_appl(x)));
    }
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct identifier_string_traverser: public add_traverser_identifier_strings<data::identifier_string_traverser, Derived>
{
  typedef add_traverser_identifier_strings<data::identifier_string_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_traverser_identifier_strings code ---//

//--- start generated add_traverser_action_labels code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_action_labels: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  template <typename Container>
  void operator()(const pbes_system::pbes<Container>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.equations());
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct action_label_traverser: public add_traverser_action_labels<lps::action_label_traverser, Derived>
{
  typedef add_traverser_action_labels<lps::action_label_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_traverser_action_labels code ---//

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TRAVERSER_H
