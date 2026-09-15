// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/check_formula_actions.h
/// \brief Checks that the actions of a state formula occur in a specification.

#ifndef MCRL2_MODAL_FORMULA_CHECK_FORMULA_ACTIONS_H
#define MCRL2_MODAL_FORMULA_CHECK_FORMULA_ACTIONS_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2::state_formulas
{

/// \brief Prints a warning if formula contains an action that does not occur in spec_actions.
/// \param[in] formula A state formula.
/// \param[in] spec_actions The action labels that occur in the specification (e.g. an LTS or LPS).
/// \param[in] spec_name The name of the specification, used in the warning message.
inline void check_formula_actions(const state_formula& formula,
  const std::set<process::action_label>& spec_actions,
  const std::string& spec_name)
{
  std::set<process::action_label> used_state_formula_actions = state_formulas::find_action_labels(formula);
  std::set<process::action_label> diff = utilities::detail::set_difference(used_state_formula_actions, spec_actions);
  if (!diff.empty())
  {
    mCRL2log(log::warning) << "Warning: the modal formula contains actions " << core::detail::print_list(diff)
                           << " that are in the data specification, but do not appear in the " << spec_name << "!"
                           << std::endl;
  }
}

} // namespace mcrl2::state_formulas

#endif // MCRL2_MODAL_FORMULA_CHECK_FORMULA_ACTIONS_H