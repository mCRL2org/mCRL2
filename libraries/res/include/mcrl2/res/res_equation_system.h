// Author(s): Jan Friso Groote. Based on boolean_equation_system by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/res/res_equation_system.h
/// \brief This file defines real equation systems. 

#ifndef MCRL2_RES_RES_EQUATION_SYSTEM_H
#define MCRL2_RES_RES_EQUATION_SYSTEM_H

#include "mcrl2/res/res_equation.h"

namespace mcrl2
{

namespace res
{

// forward declarations
class res_equation_system;

atermpp::aterm_appl res_equation_system_to_aterm(const res_equation_system& p);

/// \brief res equation system
// <RES>          ::= RES(<BooleanEquation>*, <BooleanExpression>)
class res_equation_system
{
  public:
    typedef res_equation equation_type;

  protected:
    /// \brief The equations
    std::vector<res_equation> m_equations;

    /// \brief The initial state
    res_expression m_initial_state;

  public:
    /// \brief Constructor.
    res_equation_system()
      : m_initial_state(core::term_traits<res_expression>::true_())
    {}

    /// \brief Constructor.
    /// \param equations A sequence of res equations
    /// \param initial_state An initial state
    res_equation_system(
      const std::vector<res_equation>& equations,
      res_expression initial_state)
      :
      m_equations(equations),
      m_initial_state(initial_state)
    {}

    /// \brief Returns the equations.
    /// \return The equations
    const std::vector<res_equation>& equations() const
    {
      return m_equations;
    }

    /// \brief Returns the equations.
    /// \return The equations
    std::vector<res_equation>& equations()
    {
      return m_equations;
    }

    /// \brief Returns the initial state.
    /// \return The initial state.
    const res_expression& initial_state() const
    {
      return m_initial_state;
    }

    /// \brief Returns the initial state.
    /// \return The initial state.
    res_expression& initial_state()
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

    /// \brief Returns the set of binding variables of the res_equation_system, i.e. the
    /// variables that occur on the left hand side of an equation.
    /// \return The binding variables of the equation system
    std::set<res_variable> binding_variables() const
    {
      std::set<res_variable> result;
      for (const res_equation& eqn: equations())
      {
        result.insert(eqn.variable());
      }
      return result;
    }

    /// \brief Returns the set of occurring variables of the res_equation_system, i.e.
    /// the variables that occur in the right hand side of an equation or in the
    /// initial state.
    /// \return The occurring variables of the equation system
    std::set<res_variable> occurring_variables() const;

    /// \brief Returns true if all occurring variables are binding variables.
    /// \return True if the equation system is closed
    bool is_closed() const
    {
      std::set<res_variable> bnd = binding_variables();
      std::set<res_variable> occ = occurring_variables();
      return std::includes(bnd.begin(), bnd.end(), occ.begin(), occ.end()) && bnd.find(res_variable(initial_state())) != bnd.end();
    }
};

//--- start generated class res_equation_system ---//
// prototype declaration
std::string pp(const res_equation_system& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const res_equation_system& x)
{
  return out << res::pp(x);
}
//--- end generated class res_equation_system ---//

inline
bool operator==(const res_equation_system& x, const res_equation_system& y)
{
	return x.equations() == y.equations() && x.initial_state() == y.initial_state();
}

} // namespace res

} // namespace mcrl2

#endif // MCRL2_RES_RES_EQUATION_SYSTEM_H
