// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/state_formula_specification.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_STATE_FORMULA_SPECIFICATION_H
#define MCRL2_MODAL_FORMULA_STATE_FORMULA_SPECIFICATION_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/process/action_label.h"
#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2 {

namespace state_formulas {

class state_formula_specification
{
  protected:
    /// \brief The data specification of the specification
    data::data_specification m_data;

    /// \brief The action specification of the specification
    process::action_label_list m_action_labels;

    /// \brief The set of global variables
    std::set<data::variable> m_global_variables;

    /// \brief The formula of the specification
    state_formula m_formula;

    /// \brief Initializes the specification with an aterm.
    /// \param t A term
    void construct_from_aterm(const atermpp::aterm_appl& t)
    {
      atermpp::aterm_appl::iterator i = t.begin();
      m_data            = data::data_specification(atermpp::aterm_appl(*i++));
      m_action_labels   = static_cast<process::action_label_list>(atermpp::aterm_appl(*i++)[0]);
      data::variable_list global_variables = static_cast<data::variable_list>(atermpp::aterm_appl(*i++)[0]);
      m_global_variables = std::set<data::variable>(global_variables.begin(),global_variables.end());
      atermpp::aterm_appl formula = atermpp::aterm_appl(*i);
      m_formula = state_formula(atermpp::aterm_appl(formula[0]));
    }

  public:
    /// \brief Constructor.
    state_formula_specification()
    {}

    /// \brief Constructor.
    /// \param t A term containing an aterm representation of a state formula specification.
    /// \param data_specification_is_type_checked A boolean that indicates whether the
    ///         data specification has been type checked. If so, the internal data specification
    ///         data structures will be set up. Otherwise, the function
    ///         declare_data_specification_to_be_type_checked must be invoked after type checking,
    ///         before the data specification can be used.
    state_formula_specification(const atermpp::aterm_appl& t, bool data_specification_is_type_checked = true)
    {
      // assert(core::detail::check_term_StateFrmSpec(t));
      construct_from_aterm(t);
      if (data_specification_is_type_checked)
      {
        m_data.declare_data_specification_to_be_type_checked();
      }
    }

    /// \brief Constructor that sets the global variables to empty;
    state_formula_specification(const data::data_specification& data, const process::action_label_list& action_labels, const state_formula& formula)
      : m_data(data),
        m_action_labels(action_labels),
        m_formula(formula)
    {}

    /// \brief Constructor of a state formula specification.
    state_formula_specification(
      data::data_specification data,
      process::action_label_list action_labels,
      data::variable_list global_variables,
      state_formula formula)
      : m_data(data),
        m_action_labels(action_labels),
        m_global_variables(global_variables.begin(),global_variables.end()),
        m_formula(formula)
    {}

    /// \brief Returns the data specification
    /// \return The data specification
    const data::data_specification& data() const
    {
      return m_data;
    }

    /// \brief Returns the data specification
    /// \return The data specification
    data::data_specification& data()
    {
      return m_data;
    }

    /// \brief Returns the action label specification
    /// \return The action label specification
    const process::action_label_list& action_labels() const
    {
      return m_action_labels;
    }

    /// \brief Returns the action label specification
    /// \return The action label specification
    process::action_label_list& action_labels()
    {
      return m_action_labels;
    }

    /// \brief Returns the declared free variables of the state formula specification.
    /// \return The declared free variables of the state formula specification.
    const std::set<data::variable>& global_variables() const
    {
      return m_global_variables;
    }

    /// \brief Returns the declared free variables of the state formula specification.
    /// \return The declared free variables of the state formula specification.
    std::set<data::variable>& global_variables()
    {
      return m_global_variables;
    }

    /// \brief Returns the formula of the state formula specification
    /// \return The formula of the state formula specification
    const state_formula& formula() const
    {
      return m_formula;
    }

    /// \brief Returns the formula of the state formula specification
    /// \return The formula of the state formula specification
    state_formula& formula()
    {
      return m_formula;
    }
};

//--- start generated classes ---//
// prototype declaration
std::string pp(const state_formula_specification& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const state_formula_specification& x)
{
  return out << state_formulas::pp(x);
}
//--- end generated classes ---//

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_STATE_FORMULA_SPECIFICATION_H
