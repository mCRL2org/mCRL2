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
#include "mcrl2/process/traverser.h"
#include "mcrl2/process/untyped_action.h"
#include "mcrl2/lps/untyped_multi_action.h"
#include "mcrl2/lps/stochastic_specification.h"

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
  using super::apply;

  void apply(const lps::deadlock& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (x.has_time())
    {
      static_cast<Derived&>(*this).apply(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    if (x.has_time())
    {
      static_cast<Derived&>(*this).apply(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::untyped_multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::deadlock_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.summation_variables());
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.deadlock());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.summation_variables());
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.multi_action());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::process_initializer& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.process_parameters());
    static_cast<Derived&>(*this).apply(x.deadlock_summands());
    static_cast<Derived&>(*this).apply(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_labels());
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.process());
    static_cast<Derived&>(*this).apply(x.initial_process());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_distribution& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    if (x.is_defined()) { static_cast<Derived&>(*this).apply(x.distribution()); }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.summation_variables());
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.multi_action());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.distribution());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.process_parameters());
    static_cast<Derived&>(*this).apply(x.deadlock_summands());
    static_cast<Derived&>(*this).apply(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_labels());
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.process());
    static_cast<Derived&>(*this).apply(x.initial_process());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_process_initializer& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.distribution());
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct sort_expression_traverser: public add_traverser_sort_expressions<process::sort_expression_traverser, Derived>
{
  typedef add_traverser_sort_expressions<process::sort_expression_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
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

  void apply(const lps::deadlock& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (x.has_time())
    {
      static_cast<Derived&>(*this).apply(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    if (x.has_time())
    {
      static_cast<Derived&>(*this).apply(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::untyped_multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::deadlock_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.deadlock());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.multi_action());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::process_initializer& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.deadlock_summands());
    static_cast<Derived&>(*this).apply(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.process());
    static_cast<Derived&>(*this).apply(x.initial_process());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_distribution& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (x.is_defined()) { static_cast<Derived&>(*this).apply(x.distribution()); }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.multi_action());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.distribution());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.deadlock_summands());
    static_cast<Derived&>(*this).apply(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.process());
    static_cast<Derived&>(*this).apply(x.initial_process());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_process_initializer& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.distribution());
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct data_expression_traverser: public add_traverser_data_expressions<process::data_expression_traverser, Derived>
{
  typedef add_traverser_data_expressions<process::data_expression_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
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

  void apply(const lps::deadlock& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (x.has_time())
    {
      static_cast<Derived&>(*this).apply(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    if (x.has_time())
    {
      static_cast<Derived&>(*this).apply(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::untyped_multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::deadlock_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.summation_variables());
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.deadlock());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.summation_variables());
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.multi_action());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::process_initializer& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.process_parameters());
    static_cast<Derived&>(*this).apply(x.deadlock_summands());
    static_cast<Derived&>(*this).apply(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.process());
    static_cast<Derived&>(*this).apply(x.initial_process());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_distribution& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    if (x.is_defined()) { static_cast<Derived&>(*this).apply(x.distribution()); }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.summation_variables());
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.multi_action());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.distribution());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.process_parameters());
    static_cast<Derived&>(*this).apply(x.deadlock_summands());
    static_cast<Derived&>(*this).apply(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.process());
    static_cast<Derived&>(*this).apply(x.initial_process());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_process_initializer& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.distribution());
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct variable_traverser: public add_traverser_variables<process::variable_traverser, Derived>
{
  typedef add_traverser_variables<process::variable_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
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

  void apply(const lps::deadlock& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (x.has_time())
    {
      static_cast<Derived&>(*this).apply(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    if (x.has_time())
    {
      static_cast<Derived&>(*this).apply(x.time());
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::untyped_multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::deadlock_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.summation_variables());
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.deadlock());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.summation_variables());
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.multi_action());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::process_initializer& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.process_parameters());
    static_cast<Derived&>(*this).apply(x.deadlock_summands());
    static_cast<Derived&>(*this).apply(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_labels());
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.process());
    static_cast<Derived&>(*this).apply(x.initial_process());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_distribution& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.variables());
    if (x.is_defined()) { static_cast<Derived&>(*this).apply(x.distribution()); }
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.summation_variables());
    static_cast<Derived&>(*this).apply(x.condition());
    static_cast<Derived&>(*this).apply(x.multi_action());
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.distribution());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.process_parameters());
    static_cast<Derived&>(*this).apply(x.deadlock_summands());
    static_cast<Derived&>(*this).apply(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_labels());
    static_cast<Derived&>(*this).apply(x.global_variables());
    static_cast<Derived&>(*this).apply(x.process());
    static_cast<Derived&>(*this).apply(x.initial_process());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_process_initializer& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.assignments());
    static_cast<Derived&>(*this).apply(x.distribution());
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct identifier_string_traverser: public add_traverser_identifier_strings<process::identifier_string_traverser, Derived>
{
  typedef add_traverser_identifier_strings<process::identifier_string_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated add_traverser_identifier_strings code ---//

//--- start generated add_traverser_action_labels code ---//
template <template <class> class Traverser, class Derived>
struct add_traverser_action_labels: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  void apply(const lps::multi_action& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.actions());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.multi_action());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_labels());
    static_cast<Derived&>(*this).apply(x.process());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_action_summand& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.multi_action());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_linear_process& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_summands());
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const lps::stochastic_specification& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.action_labels());
    static_cast<Derived&>(*this).apply(x.process());
    static_cast<Derived&>(*this).leave(x);
  }

};

/// \brief Traverser class
template <typename Derived>
struct action_label_traverser: public add_traverser_action_labels<process::action_label_traverser, Derived>
{
  typedef add_traverser_action_labels<process::action_label_traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
};
//--- end generated add_traverser_action_labels code ---//

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_TRAVERSER_H
