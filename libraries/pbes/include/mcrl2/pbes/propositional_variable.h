// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
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
#include "mcrl2/data/data.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/struct.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/detail/data_utility.h"

namespace mcrl2 {

namespace pbes_system {

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::term_list;
using atermpp::read_from_named_file;

/// \brief A propositional variable declaration.
///
// <PropVarDecl>  ::= PropVarDecl(<String>, <DataVarId>*)
class propositional_variable: public aterm_appl
{
  protected:
    core::identifier_string m_name;
    data::data_variable_list m_parameters;

  public:
    /// Constructor.
    ///
    propositional_variable()
      : aterm_appl(core::detail::constructPropVarDecl())
    {}

    /// Constructor.
    ///
    propositional_variable(std::string s)
    {
      std::pair<std::string, data::data_expression_list> p = data::detail::parse_variable(s);
      m_name      = core::identifier_string(p.first);
      m_parameters = p.second;
      m_term      = reinterpret_cast<ATerm>(core::detail::gsMakePropVarDecl(m_name, m_parameters));
    }

    /// Constructor.
    ///
    propositional_variable(aterm_appl t)
      : aterm_appl(t)
    {
      assert(core::detail::check_rule_PropVarDecl(m_term));
      iterator i = t.begin();
      m_name = *i++;
      m_parameters = *i;
    }

    /// Constructor.
    ///
    propositional_variable(core::identifier_string name, data::data_variable_list parameters)
      : aterm_appl(core::detail::gsMakePropVarDecl(name, parameters)),
        m_name(name),
        m_parameters(parameters)
    {
    }

    /// Returns the name of the propositional variable.
    ///
    core::identifier_string name() const
    {
      return m_name;
    }

    /// Returns the parameters of the propositional variable.
    ///
    data::data_variable_list parameters() const
    {
      return m_parameters;
    }
};

/// \brief singly linked list of propositional variable declarations
///
typedef term_list<propositional_variable> propositional_variable_list;

/// \brief Returns true if the term t is a propositional variable declaration
inline
bool is_propositional_variable(aterm_appl t)
{
  return core::detail::gsIsPropVarDecl(t);
}

/// \brief A propositional variable instantiation.
///
// <PropVarInst>  ::= PropVarInst(<String>, <DataExpr>*)
class propositional_variable_instantiation: public aterm_appl
{
  protected:
    core::identifier_string m_name;
    data::data_expression_list m_parameters;

  public:
    /// Constructor.
    ///
    propositional_variable_instantiation()
      : aterm_appl(core::detail::constructPropVarInst())
    {}

    /// Constructor.
    ///
    propositional_variable_instantiation(std::string s)
    {
      std::pair<std::string, data::data_expression_list> p = data::detail::parse_variable(s);
      m_name        = core::identifier_string(p.first);
      m_parameters = p.second;
      m_term        = reinterpret_cast<ATerm>(core::detail::gsMakePropVarInst(m_name, m_parameters));
    }

    /// Constructor.
    ///
    propositional_variable_instantiation(aterm_appl t)
      : aterm_appl(t)
    {
      assert(core::detail::check_rule_PropVarInst(m_term));
      iterator i = t.begin();
      m_name = *i++;
      m_parameters = *i;
    }

    /// Constructor.
    ///
    propositional_variable_instantiation(core::identifier_string name, data::data_expression_list parameters)
      : aterm_appl(core::detail::gsMakePropVarInst(name, parameters)),
        m_name(name),
        m_parameters(parameters)
    {
    }

    /// Returns the term containing the name of the propositional variable.
    ///
    core::identifier_string name() const
    {
      return m_name;
    }

    /// Returns the parameters of the propositional variable.
    ///
    data::data_expression_list parameters() const
    {
      return m_parameters;
    }
};

/// \brief singly linked list of propositional variables instantiations
///
typedef term_list<propositional_variable_instantiation> propositional_variable_instantiation_list;

/// \brief Returns true if the term t is a propositional variable instantiation
inline
bool is_propositional_variable_instantiation(aterm_appl t)
{
  return core::detail::gsIsPropVarInst(t);
}

} // namespace pbes_system

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::pbes_system::propositional_variable;
using mcrl2::pbes_system::propositional_variable_instantiation;

template<>
struct aterm_traits<propositional_variable>
{
  typedef ATermAppl aterm_type;
  static void protect(propositional_variable t)   { t.protect(); }
  static void unprotect(propositional_variable t) { t.unprotect(); }
  static void mark(propositional_variable t)      { t.mark(); }
  static ATerm term(propositional_variable t)     { return t.term(); }
  static ATerm* ptr(propositional_variable& t)    { return &t.term(); }
};

template<>
struct aterm_traits<propositional_variable_instantiation>
{
  typedef ATermAppl aterm_type;
  static void protect(propositional_variable_instantiation t)   { t.protect(); }
  static void unprotect(propositional_variable_instantiation t) { t.unprotect(); }
  static void mark(propositional_variable_instantiation t)      { t.mark(); }
  static ATerm term(propositional_variable_instantiation t)     { return t.term(); }
  static ATerm* ptr(propositional_variable_instantiation& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_PBES_PROPOSITIONAL_VARIABLE_H
