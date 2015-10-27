// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/boolean_equation_system.h
/// \brief add your file description here.

#ifndef MCRL2_BES_BOOLEAN_EQUATION_SYSTEM_H
#define MCRL2_BES_BOOLEAN_EQUATION_SYSTEM_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include "mcrl2/bes/boolean_equation.h"
#include "mcrl2/bes/detail/io.h"
#include "mcrl2/core/load_aterm.h"
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2
{

namespace bes
{

// forward declarations
class boolean_equation_system;

template <typename Object, typename OutputIterator>
void find_boolean_variables(const Object& x, OutputIterator o);

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

    /// \brief Initialize the boolean_equation_system with an atermpp::aterm_appl.
    /// \param t A term
    void init_term(atermpp::aterm_appl t)
    {
      atermpp::aterm_appl::iterator i = t.begin();
      atermpp::term_list<atermpp::aterm_appl> eqn = static_cast<atermpp::term_list<atermpp::aterm_appl> >(*i++);
      m_initial_state = boolean_expression(*i);
      m_equations.reserve(eqn.size());
      for (atermpp::term_list<atermpp::aterm_appl>::const_iterator j = eqn.begin(); j != eqn.end(); ++j)
      {
        m_equations.push_back(boolean_equation(*j));
      }
    }

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

    /// \brief Reads the boolean equation system from a stream.
    /// \param stream The stream to read from.
    /// \param source The source from which the stream originates. Used for error messages.
    void load(std::istream& stream, bool binary = true, const std::string& source = "")
    {
      atermpp::aterm t = core::load_aterm(stream, binary, "BES", source);
      t = bes::detail::add_index(t);
      if (!t.type_is_appl() || !core::detail::check_rule_BES(atermpp::aterm_appl(t)))
      {
        throw mcrl2::runtime_error("The loaded ATerm is not a BES.");
      }
      init_term(atermpp::aterm_appl(t));
      if (!is_well_typed())
      {
        throw mcrl2::runtime_error("boolean equation system is not well typed (boolean_equation_system::load())");
      }
    }

    /// \brief Writes the boolean equation system to a stream.
    /// \param binary If binary is true the boolean equation system is saved in compressed binary format.
    /// Otherwise an ascii representation is saved. In general the binary format is
    /// much more compact than the ascii representation.
    /// \param stream An output stream
    /// \param binary If true, the file is saved in binary format
    void save(std::ostream& stream, bool binary = true) const
    {
      assert(is_well_typed());
      if (binary)
      {
        atermpp::write_term_to_binary_stream(bes::detail::remove_index(boolean_equation_system_to_aterm(*this)), stream);
      }
      else
      {
        atermpp::write_term_to_text_stream(bes::detail::remove_index(boolean_equation_system_to_aterm(*this)), stream);
      }
    }

    /// \brief Returns the set of binding variables of the boolean_equation_system, i.e. the
    /// variables that occur on the left hand side of an equation.
    /// \return The binding variables of the equation system
    std::set<boolean_variable> binding_variables() const
    {
      std::set<boolean_variable> result;
      for (auto i = equations().begin(); i != equations().end(); ++i)
      {
        result.insert(i->variable());
      }
      return result;
    }

    /// \brief Returns the set of occurring variables of the boolean_equation_system, i.e.
    /// the variables that occur in the right hand side of an equation or in the
    /// initial state.
    /// \return The occurring variables of the equation system
    std::set<boolean_variable> occurring_variables() const
    {
      std::set<boolean_variable> result;
      for (auto i = m_equations.begin(); i != m_equations.end(); ++i)
      {
        find_boolean_variables(i->formula(), std::inserter(result, result.end()));
      }
      find_boolean_variables(m_initial_state, std::inserter(result, result.end()));
      return result;
    }

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

/// \brief Conversion to aterm_appl.
/// \return The boolean equation system converted to aterm format.
inline
atermpp::aterm_appl boolean_equation_system_to_aterm(const boolean_equation_system& p)
{
  atermpp::aterm_list eqn_list;
  const std::vector<boolean_equation>& eqn = p.equations();
  for (auto i = eqn.rbegin(); i != eqn.rend(); ++i)
  {
    atermpp::aterm a = boolean_equation_to_aterm(*i);
    eqn_list.push_front(a);
  }
  return atermpp::aterm_appl(core::detail::function_symbol_BES(), eqn_list, p.initial_state());
}

} // namespace bes

} // namespace mcrl2

#ifndef MCRL2_BES_FIND_H
#include "mcrl2/bes/find.h"
#endif

#endif // MCRL2_BES_BOOLEAN_EQUATION_SYSTEM_H
