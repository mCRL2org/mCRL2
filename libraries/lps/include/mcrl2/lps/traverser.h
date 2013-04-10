// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/traverser.h
/// \brief add your file description here.

// To avoid circular inclusion problems
#ifndef MCRL2_LPS_SPECIFICATION_H
#include "mcrl2/lps/specification.h"
#endif

#ifndef MCRL2_LPS_TRAVERSER_H
#define MCRL2_LPS_TRAVERSER_H

#include "mcrl2/data/traverser.h"
#include "mcrl2/lps/untyped_action.h"

namespace mcrl2
{

namespace lps
{

//--- start generated add_traverser_sort_expressions code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_sort_expressions: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const lps::action_label& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.sorts());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.label());
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::untyped_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::deadlock& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (x.has_time())
    {
      static_cast<Derived&>(*this)(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.actions());
    if (x.has_time())
    {
      static_cast<Derived&>(*this)(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::deadlock_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.summation_variables());
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.deadlock());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.summation_variables());
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.multi_action());
    static_cast<Derived&>(*this)(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::process_initializer& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.process_parameters());
    static_cast<Derived&>(*this)(x.deadlock_summands());
    static_cast<Derived&>(*this)(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.action_labels());
    static_cast<Derived&>(*this)(x.global_variables());
    static_cast<Derived&>(*this)(x.process());
    static_cast<Derived&>(*this)(x.initial_process());
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

  void operator()(const lps::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::untyped_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::deadlock& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (x.has_time())
    {
      static_cast<Derived&>(*this)(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.actions());
    if (x.has_time())
    {
      static_cast<Derived&>(*this)(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::deadlock_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.deadlock());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.multi_action());
    static_cast<Derived&>(*this)(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::process_initializer& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.deadlock_summands());
    static_cast<Derived&>(*this)(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.process());
    static_cast<Derived&>(*this)(x.initial_process());
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

//--- start generated add_traverser_variables code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_variables: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  void operator()(const lps::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::untyped_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::deadlock& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (x.has_time())
    {
      static_cast<Derived&>(*this)(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.actions());
    if (x.has_time())
    {
      static_cast<Derived&>(*this)(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::deadlock_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.summation_variables());
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.deadlock());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.summation_variables());
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.multi_action());
    static_cast<Derived&>(*this)(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::process_initializer& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.process_parameters());
    static_cast<Derived&>(*this)(x.deadlock_summands());
    static_cast<Derived&>(*this)(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.global_variables());
    static_cast<Derived&>(*this)(x.process());
    static_cast<Derived&>(*this)(x.initial_process());
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

  void operator()(const lps::action_label& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.name());
    static_cast<Derived&>(*this)(x.sorts());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.label());
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::untyped_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.name());
    static_cast<Derived&>(*this)(x.arguments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::deadlock& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (x.has_time())
    {
      static_cast<Derived&>(*this)(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.actions());
    if (x.has_time())
    {
      static_cast<Derived&>(*this)(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::deadlock_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.summation_variables());
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.deadlock());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.summation_variables());
    static_cast<Derived&>(*this)(x.condition());
    static_cast<Derived&>(*this)(x.multi_action());
    static_cast<Derived&>(*this)(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::process_initializer& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.process_parameters());
    static_cast<Derived&>(*this)(x.deadlock_summands());
    static_cast<Derived&>(*this)(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.action_labels());
    static_cast<Derived&>(*this)(x.global_variables());
    static_cast<Derived&>(*this)(x.process());
    static_cast<Derived&>(*this)(x.initial_process());
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

  void operator()(const lps::action_label& x)
  {
    static_cast<Derived&>(*this).enter(x);
    // skip
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.label());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.multi_action());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const lps::specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this)(x.action_labels());
    static_cast<Derived&>(*this)(x.process());
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct action_label_traverser: public add_traverser_action_labels<core::traverser, Derived>
{
  typedef add_traverser_action_labels<core::traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};
//--- end generated add_traverser_action_labels code ---//

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_TRAVERSER_H
