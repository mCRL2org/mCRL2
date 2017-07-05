// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_TYPECHECK_H
#define MCRL2_MODAL_FORMULA_TYPECHECK_H

#include "mcrl2/data/bag.h"
#include "mcrl2/data/fbag.h"
#include "mcrl2/data/fset.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/modal_formula/builder.h"
#include "mcrl2/modal_formula/detail/state_variable_context.h"
#include "mcrl2/modal_formula/is_monotonous.h"
#include "mcrl2/modal_formula/normalize_sorts.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/state_formula_specification.h"
#include "mcrl2/process/typecheck.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2
{

namespace action_formulas
{

namespace detail
{

struct typecheck_builder: public action_formula_builder<typecheck_builder>
{
  typedef action_formula_builder<typecheck_builder> super;
  using super::apply;

  data::data_type_checker& m_data_type_checker;
  data::detail::variable_context m_variable_context;
  const process::detail::action_context& m_action_context;

  typecheck_builder(data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variable_context,
                    const process::detail::action_context& action_context
                   )
    : m_data_type_checker(data_typechecker),
      m_variable_context(variable_context),
      m_action_context(action_context)
  {}

  process::action typecheck_action(const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    return process::typecheck_action(name, parameters, m_data_type_checker, m_variable_context, m_action_context);
  }

  action_formula apply(const data::data_expression& x)
  {
    return m_data_type_checker.typecheck_data_expression(x, data::sort_bool::bool_(), m_variable_context);
  }

  action_formula apply(const action_formulas::at& x)
  {
    data::data_expression new_time = m_data_type_checker.typecheck_data_expression(x.time_stamp(), data::sort_real::real_(), m_variable_context);
    return at((*this).apply(x.operand()), new_time);
  }

  action_formula apply(const process::untyped_multi_action& x)
  {
    // If x has size 1, first try to type check it as a data expression.
    if (x.actions().size() == 1)
    {
      const data::untyped_data_parameter& y = x.actions().front();
      try
      {
        return data::typecheck_untyped_data_parameter(m_data_type_checker, y.name(), y.arguments(), data::sort_bool::bool_(), m_variable_context);
      }
      catch (mcrl2::runtime_error& )
      {
        // skip
      }
    }
    // Type check it as a multi action
    process::action_list new_arguments;
    for (const data::untyped_data_parameter& a: x.actions())
    {
      new_arguments.push_front(typecheck_action(a.name(), a.arguments()));
    }
    new_arguments = atermpp::reverse(new_arguments);
    return action_formulas::multi_action(new_arguments);
  }

  action_formula apply(const action_formulas::forall& x)
  {
    try
    {
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      action_formula body = (*this).apply(x.body());
      m_variable_context = m_variable_context_copy;
      return forall(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + action_formulas::pp(x));
    }
  }

  action_formula apply(const action_formulas::exists& x)
  {
    try
    {
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      action_formula body = (*this).apply(x.body());
      m_variable_context = m_variable_context_copy;
      return exists(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + action_formulas::pp(x));
    }
  }
};

inline
typecheck_builder make_typecheck_builder(
                    data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variables,
                    const process::detail::action_context& actions
                   )
{
  return typecheck_builder(data_typechecker, variables, actions);
}

} // namespace detail

template <typename ActionLabelContainer = std::vector<state_formulas::variable>, typename VariableContainer = std::vector<data::variable> >
action_formula typecheck_action_formula(const action_formula& x,
                                        const data::data_specification& dataspec,
                                        const VariableContainer& variables,
                                        const ActionLabelContainer& actions
                                       )
{
  data::data_type_checker data_typechecker(dataspec);
  data::detail::variable_context variable_context;
  variable_context.add_context_variables(variables, data_typechecker);
  process::detail::action_context action_context;
  action_context.add_context_action_labels(actions, data_typechecker);
  return detail::make_typecheck_builder(data_typechecker, variable_context, action_context).apply(action_formulas::normalize_sorts(x, data_typechecker.typechecked_data_specification()));
}

inline
action_formula typecheck_action_formula(const action_formula& x, const lps::specification& lpsspec)
{
  return typecheck_action_formula(x, lpsspec.data(), lpsspec.global_variables(), lpsspec.action_labels());
}

} // namespace action_formulas

namespace regular_formulas
{

namespace detail
{

struct typecheck_builder: public regular_formula_builder<typecheck_builder>
{
  typedef regular_formula_builder<typecheck_builder> super;
  using super::apply;

  data::data_type_checker& m_data_type_checker;
  const data::detail::variable_context& m_variable_context;
  const process::detail::action_context& m_action_context;

  typecheck_builder(data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variables,
                    const process::detail::action_context& actions
                   )
    : m_data_type_checker(data_typechecker),
      m_variable_context(variables),
      m_action_context(actions)
  {}

  data::data_expression make_fbag_union(const data::data_expression& left, const data::data_expression& right)
  {
    const data::sort_expression& s = atermpp::down_cast<data::application>(left).head().sort();
    const data::container_sort& cs = atermpp::down_cast<data::container_sort>(s);
    return data::sort_fbag::union_(cs.element_sort(), left, right);
  }

  data::data_expression make_bag_union(const data::data_expression& left, const data::data_expression& right)
  {
    const data::sort_expression& s = atermpp::down_cast<data::application>(left).head().sort();
    const data::container_sort& cs = atermpp::down_cast<data::container_sort>(s);
    return data::sort_bag::union_(cs.element_sort(), left, right);
  }

  data::data_expression make_fset_union(const data::data_expression& left, const data::data_expression& right)
  {
    const data::sort_expression& s = atermpp::down_cast<data::application>(left).head().sort();
    const data::container_sort& cs = atermpp::down_cast<data::container_sort>(s);
    return data::sort_fset::union_(cs.element_sort(), left, right);
  }

  data::data_expression make_set_union(const data::data_expression& left, const data::data_expression& right)
  {
    const data::sort_expression& s = atermpp::down_cast<data::application>(left).head().sort();
    const data::container_sort& cs = atermpp::down_cast<data::container_sort>(s);
    return data::sort_set::union_(cs.element_sort(), left, right);
  }

  data::data_expression make_plus(const data::data_expression& left, const data::data_expression& right)
  {
    if (data::sort_real::is_real(left.sort()) || data::sort_real::is_real(right.sort()))
    {
      return data::sort_real::plus(left, right);
    }
    else if (data::sort_int::is_int(left.sort()) || data::sort_int::is_int(right.sort()))
    {
      return data::sort_int::plus(left, right);
    }
    else if (data::sort_nat::is_nat(left.sort()) || data::sort_nat::is_nat(right.sort()))
    {
      return data::sort_nat::plus(left, right);
    }
    else if (data::sort_pos::is_pos(left.sort()) || data::sort_pos::is_pos(right.sort()))
    {
      return data::sort_pos::plus(left, right);
    }
    else if (data::sort_bag::is_union_application(left) || data::sort_bag::is_union_application(right))
    {
      return make_bag_union(left, right);
    }
    else if (data::sort_fbag::is_union_application(left) || data::sort_fbag::is_union_application(right))
    {
      return make_fbag_union(left, right);
    }
    else if (data::sort_set::is_union_application(left) || data::sort_set::is_union_application(right))
    {
      return make_set_union(left, right);
    }
    else if (data::sort_fset::is_union_application(left) || data::sort_fset::is_union_application(right))
    {
      return make_fset_union(left, right);
    }
    throw mcrl2::runtime_error("could not typecheck " + data::pp(left) + " + " + data::pp(right));
  }

  data::data_expression make_element_at(const data::data_expression& left, const data::data_expression& right) const
  {
    const data::sort_expression& s = atermpp::down_cast<data::application>(left).head().sort();
    const data::container_sort& cs = atermpp::down_cast<data::container_sort>(s);
    return data::sort_list::element_at(cs.element_sort(), left, right);
  }

  regular_formula apply(const regular_formulas::untyped_regular_formula& x)
  {
    regular_formula left = (*this).apply(x.left());
    regular_formula right = (*this).apply(x.right());
    if (data::is_data_expression(left) && data::is_data_expression(right))
    {
      if (x.name() == core::identifier_string("."))
      {
        return make_element_at(atermpp::down_cast<data::data_expression>(left), atermpp::down_cast<data::data_expression>(right));
      }
      else
      {
        return make_plus(atermpp::down_cast<data::data_expression>(left), atermpp::down_cast<data::data_expression>(right));
      }
    }
    else
    {
      if (x.name() == core::identifier_string("."))
      {
        return seq(left, right);
      }
      else
      {
        return alt(left, right);
      }
    }
  }

  regular_formula apply(const action_formulas::action_formula& x)
  {
    return action_formulas::detail::make_typecheck_builder(m_data_type_checker, m_variable_context, m_action_context).apply(x);
  }
};

inline
typecheck_builder make_typecheck_builder(
                    data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variables,
                    const process::detail::action_context& actions
                   )
{
  return typecheck_builder(data_typechecker, variables, actions);
}

} // namespace detail

template <typename ActionLabelContainer = std::vector<state_formulas::variable>, typename VariableContainer = std::vector<data::variable> >
regular_formula typecheck_regular_formula(const regular_formula& x,
                                          const data::data_specification& dataspec,
                                          const VariableContainer& variables,
                                          const ActionLabelContainer& actions
                                         )
{
  data::data_type_checker data_typechecker(dataspec);
  data::detail::variable_context variable_context;
  variable_context.add_context_variables(variables, data_typechecker);
  process::detail::action_context action_context;
  action_context.add_context_action_labels(actions, data_typechecker);
  return detail::make_typecheck_builder(data_typechecker, variable_context, action_context).apply(regular_formulas::normalize_sorts(x, data_typechecker.typechecked_data_specification()));
}

inline
regular_formula typecheck_regular_formula(const regular_formula& x, const lps::specification& lpsspec)
{
  return typecheck_regular_formula(x, lpsspec.data(), lpsspec.global_variables(), lpsspec.action_labels());
}

} // namespace regular_formulas

namespace state_formulas
{

namespace detail
{

struct typecheck_builder: public state_formula_builder<typecheck_builder>
{
  typedef state_formula_builder<typecheck_builder> super;
  using super::apply;

  data::data_type_checker& m_data_type_checker;
  data::detail::variable_context m_variable_context;
  const process::detail::action_context& m_action_context;
  detail::state_variable_context m_state_variable_context;

  typecheck_builder(data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variable_context,
                    const process::detail::action_context& action_context,
                    const detail::state_variable_context& state_variable_context
                   )
    : m_data_type_checker(data_typechecker),
      m_variable_context(variable_context),
      m_action_context(action_context),
      m_state_variable_context(state_variable_context)
  {}

  state_formula apply(const data::data_expression& x)
  {
    return m_data_type_checker.typecheck_data_expression(x, data::sort_bool::bool_(), m_variable_context);
  }

  state_formula apply(const state_formulas::forall& x)
  {
    try
    {
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      state_formula body = (*this).apply(x.body());
      m_variable_context = m_variable_context_copy;
      return forall(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + state_formulas::pp(x));
    }
  }

  state_formula apply(const state_formulas::exists& x)
  {
    try
    {
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      state_formula body = (*this).apply(x.body());
      m_variable_context = m_variable_context_copy;
      return exists(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + state_formulas::pp(x));
    }
  }

  state_formula apply(const state_formulas::may& x)
  {
    return may(regular_formulas::detail::make_typecheck_builder(m_data_type_checker, m_variable_context, m_action_context).apply(x.formula()), (*this).apply(x.operand()));
  }

  state_formula apply(const state_formulas::must& x)
  {
    return must(regular_formulas::detail::make_typecheck_builder(m_data_type_checker, m_variable_context, m_action_context).apply(x.formula()), (*this).apply(x.operand()));
  }

  state_formula apply(const state_formulas::delay_timed& x)
  {
    data::data_expression new_time = m_data_type_checker.typecheck_data_expression(x.time_stamp(), data::sort_real::real_(), m_variable_context);
    return delay_timed(new_time);
  }

  state_formula apply(const state_formulas::yaled_timed& x)
  {
    data::data_expression new_time = m_data_type_checker.typecheck_data_expression(x.time_stamp(), data::sort_real::real_(), m_variable_context);
    return yaled_timed(new_time);
  }

  state_formula apply_untyped_parameter(const core::identifier_string& name, const data::data_expression_list& arguments)
  {
    data::sort_expression_list expected_sorts = m_state_variable_context.matching_state_variable_sorts(name, arguments);
    data::data_expression_list new_arguments;
    auto q1 = expected_sorts.begin();
    auto q2 = arguments.begin();
    for (; q1 != expected_sorts.end(); ++q1, ++q2)
    {
      new_arguments.push_front(m_data_type_checker.typecheck_data_expression(*q2, *q1, m_variable_context));
    }
    new_arguments = atermpp::reverse(new_arguments);
    return state_formulas::variable(name, new_arguments);
  }

  state_formula apply(const state_formulas::variable& x)
  {
    return apply_untyped_parameter(x.name(), x.arguments());
  }

  state_formula apply(const data::untyped_data_parameter& x)
  {
    return apply_untyped_parameter(x.name(), x.arguments());
  }

  data::variable_list assignment_variables(const data::assignment_list& x) const
  {
    std::vector<data::variable> result;
    for (const data::assignment& a: x)
    {
      result.push_back(a.lhs());
    }
    return data::variable_list(result.begin(), result.end());
  }

  template <typename MuNuFormula>
  state_formula apply_mu_nu(const MuNuFormula& x, bool is_mu)
  {
    data::assignment_list new_assignments = m_data_type_checker.typecheck_assignment_list(x.assignments(), m_variable_context);

    // add the assignment variables to the context
    auto m_variable_context_copy = m_variable_context;
    data::variable_list x_variables = assignment_variables(x.assignments());
    m_variable_context.add_context_variables(x_variables, m_data_type_checker);

    // add x to the state variable context
    auto m_state_variable_context_copy = m_state_variable_context;
    m_state_variable_context.add_state_variable(x.name(), x_variables, m_data_type_checker);

    // typecheck the operand
    state_formula new_operand = (*this).apply(x.operand());

    // restore the context
    m_variable_context = m_variable_context_copy;
    m_state_variable_context = m_state_variable_context_copy;

    if (is_mu)
    {
      return mu(x.name(), new_assignments, new_operand);
    }
    else
    {
      return nu(x.name(), new_assignments, new_operand);
    }
  }

  state_formula apply(const state_formulas::nu& x)
  {
    return apply_mu_nu(x, false);
  }

  state_formula apply(const state_formulas::mu& x)
  {
    return apply_mu_nu(x, true);
  }
};

inline
typecheck_builder make_typecheck_builder(
                    data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variable_context,
                    const process::detail::action_context& action_context,
                    const detail::state_variable_context& state_variable_context
                   )
{
  return typecheck_builder(data_typechecker, variable_context, action_context, state_variable_context);
}

} // namespace detail

class state_formula_type_checker
{
  protected:
    data::data_type_checker m_data_type_checker;
    data::detail::variable_context m_variable_context;
    process::detail::action_context m_action_context;
    detail::state_variable_context m_state_variable_context;

  public:
    /** \brief     Constructor for a state_formula type checker.
     *  \param[in] dataspec The data specification against which state formulas are checked.
     *  \param[in] action_labels The data labels that can occur in a state formula.
     *  \param[in] variables A container containing variables that can occur in state formulas.
     **/
    template <typename ActionLabelContainer = std::vector<state_formulas::variable>, typename VariableContainer = std::vector<data::variable> >
    state_formula_type_checker(const data::data_specification& dataspec,
                               const ActionLabelContainer& action_labels = ActionLabelContainer(),
                               const VariableContainer& variables = VariableContainer()
                              )
      : m_data_type_checker(dataspec)
    {
      m_variable_context.add_context_variables(variables, m_data_type_checker);
      m_action_context.add_context_action_labels(action_labels, m_data_type_checker);
    }

    //check correctness of the state formula as follows:
    //1) determine the types of actions according to the definitions
    //   in spec
    //2) determine the types of data expressions according to the
    //   definitions in spec
    //3) check for name conflicts of data variable declarations in
    //   forall, exists, mu and nu quantifiers
    //4) check for monotonicity of fixpoint variables
    state_formula typecheck_state_formula(const state_formula& x)
    {
      mCRL2log(log::verbose) << "type checking state formula..." << std::endl;
      return detail::make_typecheck_builder(m_data_type_checker, m_variable_context, m_action_context, m_state_variable_context).apply(state_formulas::normalize_sorts(x, m_data_type_checker.typechecked_data_specification()));
    }
};

/** \brief     Type check a state formula.
 *  Throws an exception if something went wrong.
 *  \param[in] x A state formula that has not been type checked.
 *  \param[in] dataspec The data specification against which the formulas is checked.
 *  \param[in] action_labels A declaration of action labels that can be used in the state formulas.
 *  \param[in] variables A container with global data variables.
 *  \post      formula is type checked.
 **/
template <typename VariableContainer, typename ActionLabelContainer>
state_formula typecheck_state_formula(const state_formula& x,
                                      const data::data_specification& dataspec = data::data_specification(),
                                      const ActionLabelContainer& action_labels = ActionLabelContainer(),
                                      const VariableContainer& variables = VariableContainer()
                                     )
{
  try
  {
    state_formula_type_checker type_checker(dataspec, action_labels, variables);
    return type_checker.typecheck_state_formula(x);
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check modal formula " + state_formulas::pp(x));
  }
}

/** \brief     Type check a state formula.
 *  Throws an exception if something went wrong.
 *  \param[in] x A state formula that has not been type checked.
 *  \param[in] lpsspec A linear process specifications containing data, action and global variable declarations
 *                     to be used when typechecking the formula.
 *  \post      formula is type checked.
 **/
inline
state_formula typecheck_state_formula(const state_formula& x,
                                      const lps::specification& lpsspec
                                     )
{
  return typecheck_state_formula(x, lpsspec.data(), lpsspec.action_labels(), lpsspec.global_variables());
}

/// \brief Typecheck the state formula specification formspec. It is assumed that the formula is self contained,
/// i.e. all actions and sorts must be declared.
inline
void typecheck_state_formula_specification(state_formula_specification& formspec)
{
  try
  {
    data::data_type_checker checker(formspec.data());
    data::data_specification dataspec = checker.typechecked_data_specification();
    state_formulas::normalize_sorts(formspec, dataspec);
    state_formula_type_checker type_checker(dataspec, formspec.action_labels(), std::vector<data::variable>());
    formspec.formula() = type_checker.typecheck_state_formula(formspec.formula());
    formspec.data() = checker.typechecked_data_specification();
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check modal formula specification " + state_formulas::pp(formspec));
  }
}

/// \brief Typecheck the state formula specification formspec. It is assumed that the formula is not self contained,
/// i.e. some of the actions and sorts may be declared in lpsspec.
inline
void typecheck_state_formula_specification(state_formula_specification& formspec, const lps::specification& lpsspec)
{
  try
  {
    data::data_type_checker checker(formspec.data());
    data::data_specification dataspec = checker.typechecked_data_specification();
    state_formulas::normalize_sorts(formspec, dataspec);
    state_formula_type_checker type_checker(dataspec, lpsspec.action_labels() + formspec.action_labels(), lpsspec.global_variables());
    formspec.formula() = type_checker.typecheck_state_formula(formspec.formula());
    formspec.data() = checker.typechecked_data_specification();
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check modal formula specification " + state_formulas::pp(formspec));
  }
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_TYPECHECK_H
