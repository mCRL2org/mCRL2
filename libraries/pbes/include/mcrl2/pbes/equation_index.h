// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/equation_index.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_EQUATION_INDEX_H
#define MCRL2_PBES_EQUATION_INDEX_H

#include <map>
#include "mcrl2/core/identifier_string.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Maps variables to their corresponding equations in a boolean_equation_system or PBES.
template <typename EquationSystem>
class equation_index
{
  protected:
    // the pbes that contains the equations
    const EquationSystem& m_equations;

    // the type of the equations
    typedef typename EquationSystem::equation_type equation_type;

    // the type of the variables
    typedef typename equation_type::variable_type variable_type;

    // maps the name of an equation to the corresponding index
    std::map<core::identifier_string, std::size_t> m_index;

  public:

    /// \brief Constructor
    equation_index(const EquationSystem& p)
      : m_equations(p)
    {
      std::size_t index = 0;
      auto const& equations = p.equations();
      for (auto i = equations.begin(); i != equations.end(); ++i)
      {
        m_index[i->variable().name()] = index++;
      }
    }

    /// \brief Returns the index of the equation of the variable with the given name
    std::size_t index(const core::identifier_string& name) const
    {
      auto i = m_index.find(name);
      assert (i != m_index.end());
      return i->second;
    }

    /// \brief Returns the equation corresponding to the given variable
    const equation_type& equation(const variable_type& x) const
    {
      return *(m_equations.equations().begin() + index(x.name()));
    }

//    /// \brief Returns a const iterator of the equation of the variable with the given name
//    std::vector<equation_type>::const_iterator const_iterator(const core::identifier_string& name) const
//    {
//      return m_equations.equations().begin() + index(name);
//    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_EQUATION_INDEX_H
