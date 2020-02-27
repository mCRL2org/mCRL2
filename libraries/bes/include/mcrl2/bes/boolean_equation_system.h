// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/boolean_equation_system.h
/// \brief add your file description here.

#ifndef MCRL2_BES_BOOLEAN_EQUATION_SYSTEM_H
#define MCRL2_BES_BOOLEAN_EQUATION_SYSTEM_H

#include "mcrl2/bes/boolean_equation.h"

namespace mcrl2
{

namespace bes
{

// forward declarations
class boolean_equation_system;

atermpp::aterm_appl boolean_equation_system_to_aterm(const boolean_equation_system& p);

/// \brief boolean equation system
// <BES>          ::= BES(<BooleanEquation>*, <BooleanExpression>)
class boolean_equation_system
{
  public:
    typedef boolean_equation equation_type;

  protected:
    /// \brief The equations
    std::vector<boolean_equation> m_equations;

    /// \brief The initial state
    boolean_expression m_initial_state;

  public:
    /// \brief Constructor.
    boolean_equation_system()
      : m_initial_state(core::term_traits<boolean_expression>::true_())
    {}

    /// \brief Constructor.
    /// \param equations A sequence of boolean equations
    /// \param initial_state An initial state
    boolean_equation_system(
      const std::vector<boolean_equation>& equations,
      boolean_expression initial_state)
      :
      m_equations(equations),
      m_initial_state(initial_state)
    {}

    /// \brief Returns the equations.
    /// \return The equations
    const std::vector<boolean_equation>& equations() const
    {
      return m_equations;
    }

    /// \brief Returns the equations.
    /// \return The equations
    std::vector<boolean_equation>& equations()
    {
      return m_equations;
    }

    /// \brief Returns the initial state.
    /// \return The initial state.
    const boolean_expression& initial_state() const
    {
      return m_initial_state;
    }

    /// \brief Returns the initial state.
    /// \return The initial state.
    boolean_expression& initial_state()
    {
      return m_initial_state;
    }

    /// \brief Returns true.
    /// Some checks will be added later.
    /// \return The value true.
    bool is_well_typed() const
    {
      return true;
    }

    /// \brief Returns the set of binding variables of the boolean_equation_system, i.e. the
    /// variables that occur on the left hand side of an equation.
    /// \return The binding variables of the equation system
    std::set<boolean_variable> binding_variables() const
    {
      std::set<boolean_variable> result;
      for (const boolean_equation& eqn: equations())
      {
        result.insert(eqn.variable());
      }
      return result;
    }

    /// \brief Returns the set of occurring variables of the boolean_equation_system, i.e.
    /// the variables that occur in the right hand side of an equation or in the
    /// initial state.
    /// \return The occurring variables of the equation system
    std::set<boolean_variable> occurring_variables() const;

    /// \brief Returns true if all occurring variables are binding variables.
    /// \return True if the equation system is closed
    bool is_closed() const
    {
      std::set<boolean_variable> bnd = binding_variables();
      std::set<boolean_variable> occ = occurring_variables();
      return std::includes(bnd.begin(), bnd.end(), occ.begin(), occ.end()) && bnd.find(boolean_variable(initial_state())) != bnd.end();
    }
};

//--- start generated class boolean_equation_system ---//
// prototype declaration
std::string pp(const boolean_equation_system& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const boolean_equation_system& x)
{
  return out << bes::pp(x);
}
//--- end generated class boolean_equation_system ---//

inline
bool operator==(const boolean_equation_system& x, const boolean_equation_system& y)
{
	return x.equations() == y.equations() && x.initial_state() == y.initial_state();
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_BOOLEAN_EQUATION_SYSTEM_H
