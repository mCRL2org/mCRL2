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
#include "mcrl2/data/data_specification.h"

namespace mcrl2
{

namespace pbes_system
{

//--- start generated class propositional_variable ---//
/// \brief A propositional variable declaration
class propositional_variable: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    propositional_variable()
      : atermpp::aterm_appl(core::detail::constructPropVarDecl())
    {}

    /// \brief Constructor.
    /// \param term A term
    propositional_variable(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_PropVarDecl(*this));
    }

    /// \brief Constructor.
    propositional_variable(const core::identifier_string& name, const data::variable_list& parameters)
      : atermpp::aterm_appl(core::detail::gsMakePropVarDecl(name, parameters))
    {}

    /// \brief Constructor.
    propositional_variable(const std::string& name, const data::variable_list& parameters)
      : atermpp::aterm_appl(core::detail::gsMakePropVarDecl(core::identifier_string(name), parameters))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>(atermpp::arg1(*this));
    }

    const data::variable_list& parameters() const
    {
      return atermpp::aterm_cast<const data::variable_list>(atermpp::list_arg2(*this));
    }
//--- start user section propositional_variable ---//
    /// \brief Type of the parameters.
    typedef data::variable parameter_type;

    /// \brief Constructor.
    /// \param s A string
    propositional_variable(const std::string& s)
    {
      std::pair<std::string, data::data_expression_list> p = data::detail::parse_variable(s);
      copy_term(core::detail::gsMakePropVarDecl(core::identifier_string(p.first), atermpp::convert< data::variable_list >(p.second)));
    }
//--- end user section propositional_variable ---//
};

/// \brief list of propositional_variables
typedef atermpp::term_list<propositional_variable> propositional_variable_list;

/// \brief vector of propositional_variables
typedef std::vector<propositional_variable>    propositional_variable_vector;


/// \brief Test for a propositional_variable expression
/// \param t A term
/// \return True if it is a propositional_variable expression
inline
bool is_propositional_variable(const atermpp::aterm_appl& t)
{
  return core::detail::gsIsPropVarDecl(t);
}

//--- end generated class propositional_variable ---//

/*
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
    /// \brief Type of the parameters.
    typedef data::variable parameter_type;

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
      m_parameters = atermpp::convert< data::variable_list >(p.second);
      copy_term(core::detail::gsMakePropVarDecl(m_name, m_parameters));
    }

    /// \brief Constructor.
    /// \param t A term
    propositional_variable(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_PropVarDecl(*this));
      iterator i = t.begin();
      m_name = atermpp::aterm_cast<atermpp::aterm_string>(*i++);
      m_parameters = data::variable_list(*i);
    }

    /// \brief Constructor.
    /// \param t A term
    explicit propositional_variable(const atermpp::aterm &t1)
      : atermpp::aterm_appl(t1)
    {
      const atermpp::aterm_appl t(t1);
      assert(core::detail::check_rule_PropVarDecl(*this));
      iterator i = t.begin();
      m_name = atermpp::aterm_cast<atermpp::aterm_string>(*i++);
      m_parameters = data::variable_list(*i);
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

/// \brief Vector of propositional variable declarations
typedef std::vector<propositional_variable> propositional_variable_vector;

/// \brief Returns true if the term t is a propositional variable declaration
/// \param t A term
/// \return True if the term t is a propositional variable declaration
inline
bool is_propositional_variable(atermpp::aterm_appl t)
{
  return core::detail::gsIsPropVarDecl(t);
}
*/

// template function overloads
std::string pp(const propositional_variable& x);
std::string pp(const propositional_variable_list& x);
std::string pp(const propositional_variable_vector& x);

} // namespace pbes_system

} // namespace mcrl2

namespace std {
//--- start generated swap functions ---//
template <>
inline void swap(mcrl2::pbes_system::propositional_variable& t1, mcrl2::pbes_system::propositional_variable& t2)
{
  t1.swap(t2);
}
//--- end generated swap functions ---//
} // namespace std

#endif // MCRL2_PBES_PROPOSITIONAL_VARIABLE_H
