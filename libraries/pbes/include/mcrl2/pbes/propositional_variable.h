// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/propositional_variable.h
/// \brief The classes propositional_variable and propositional_variable_instantiation.

#ifndef MCRL2_PBES_PROPOSITIONAL_VARIABLE_H
#define MCRL2_PBES_PROPOSITIONAL_VARIABLE_H

#include <cassert>
#include <string>
#include <utility>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/parse.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief A propositional variable declaration.
// <PropVarDecl>  ::= PropVarDecl(<String>, <DataVarId>*)
class propositional_variable: public atermpp::aterm_appl
{
  protected:
    /// \brief The name of the propositional variable
    core::identifier_string m_name;

    /// \brief The parameters of the propositional variable
    data::variable_list m_parameters;

  public:
    /// \brief Constructor.
    propositional_variable()
      : atermpp::aterm_appl(core::detail::constructPropVarDecl())
    {}

    /// \brief Constructor.
    /// \param s A string
    propositional_variable(std::string s)
    {
      std::pair<std::string, data::data_expression_list> p = data::detail::parse_variable(s);
      m_name      = core::identifier_string(p.first);
      m_parameters = data::convert< data::variable_list >(p.second);
      m_term = reinterpret_cast<ATerm>(core::detail::gsMakePropVarDecl(m_name, m_parameters));
    }

    /// \brief Constructor.
    /// \param t A term
    propositional_variable(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_PropVarDecl(m_term));
      iterator i = t.begin();
      m_name = *i++;
      m_parameters = *i;
    }

    /// \brief Constructor.
    /// \param name A
    /// \param parameters A sequence of data variables
    propositional_variable(const core::identifier_string& name, const data::variable_list& parameters)
      : atermpp::aterm_appl(core::detail::gsMakePropVarDecl(name, parameters)),
        m_name(name),
        m_parameters(parameters)
    {
    }

    /// \brief Returns the name of the propositional variable.
    /// \return The name of the propositional variable.
    core::identifier_string name() const
    {
      return m_name;
    }

    /// \brief Returns the parameters of the propositional variable.
    /// \return The parameters of the propositional variable.
    data::variable_list parameters() const
    {
      return m_parameters;
    }
};

/// \brief Read-only singly linked list of propositional variable declarations
typedef atermpp::term_list<propositional_variable> propositional_variable_list;

/// \brief Returns true if the term t is a propositional variable declaration
/// \param t A term
/// \return True if the term t is a propositional variable declaration
inline
bool is_propositional_variable(atermpp::aterm_appl t)
{
  return core::detail::gsIsPropVarDecl(t);
}

/// \brief A propositional variable instantiation.
// <PropVarInst>  ::= PropVarInst(<String>, <DataExpr>*)
class propositional_variable_instantiation: public atermpp::aterm_appl
{
  protected:
    /// \brief The name of the propositional variable
    core::identifier_string m_name;

    /// \brief The parameters of the propositional variable
    data::data_expression_list m_parameters;

  public:
    /// \brief Constructor.
    propositional_variable_instantiation()
      : atermpp::aterm_appl(core::detail::constructPropVarInst())
    {}

    /// \brief Constructor.
    /// \param s A string
    propositional_variable_instantiation(std::string const& s)
    {
      std::pair<std::string, data::data_expression_list> p = data::detail::parse_variable(s);
      m_name      = core::identifier_string(p.first);
      m_parameters = data::convert< data::variable_list >(p.second);
      m_term = reinterpret_cast<ATerm>(core::detail::gsMakePropVarInst(m_name, m_parameters));
    }

    /// \brief Constructor.
    /// \param t A term
    propositional_variable_instantiation(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_PropVarInst(m_term));
      iterator i = t.begin();
      m_name = *i++;
      m_parameters = *i;
    }

    /// \brief Constructor.
    /// \param name A
    /// \param parameters A sequence of data expressions
    propositional_variable_instantiation(const core::identifier_string& name, const data::data_expression_list& parameters)
      : atermpp::aterm_appl(core::detail::gsMakePropVarInst(name, parameters)),
        m_name(name),
        m_parameters(parameters)
    {
    }

    /// \brief Returns the term containing the name of the propositional variable.
    /// \return The term containing the name of the propositional variable.
    core::identifier_string name() const
    {
      return m_name;
    }

    /// \brief Returns the parameters of the propositional variable.
    /// \return The parameters of the propositional variable.
    data::data_expression_list parameters() const
    {
      return m_parameters;
    }
};

/// \brief Read-only singly linked list of propositional variables instantiations
typedef atermpp::term_list<propositional_variable_instantiation> propositional_variable_instantiation_list;

/// \brief Returns true if the term t is a propositional variable instantiation
/// \param t A term
/// \return True if the term t is a propositional variable instantiation
inline
bool is_propositional_variable_instantiation(atermpp::aterm_appl t)
{
  return core::detail::gsIsPropVarInst(t);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PROPOSITIONAL_VARIABLE_H
