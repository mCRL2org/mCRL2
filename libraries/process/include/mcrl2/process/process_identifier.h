// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/process_identifier.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PROCESS_IDENTIFIER_H
#define MCRL2_PROCESS_PROCESS_IDENTIFIER_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/process/action_name_multiset.h"

namespace mcrl2
{

namespace process
{

/// \brief Process identifier
//<ProcVarId>    ::= ProcVarId(<String>, <SortExpr>*)
class process_identifier: public atermpp::aterm_appl
{
  public:
    /// \brief Constructor.
    process_identifier()
      : atermpp::aterm_appl(core::detail::constructProcVarId())
    {}

    /// \brief Constructor.
    /// \param term A term
    process_identifier(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_ProcVarId(m_term));
    }

    /// \brief Constructor.
    /// \param term A term
    explicit process_identifier(const ATerm &term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_ProcVarId(m_term));
    }

    /// \brief Constructor.
    process_identifier(core::identifier_string name, data::sort_expression_list sorts)
      : atermpp::aterm_appl(core::detail::gsMakeProcVarId(name, atermpp::term_list<data::sort_expression>(sorts.begin(), sorts.end())))
    {}

    /// \brief Returns the name of the process identifier
    /// \return The name of the process identifier
    core::identifier_string name() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Returns the sorts of the process identifier
    /// \return The sorts of the process identifier
    data::sort_expression_list sorts() const
    {
      return data::sort_expression_list((*this)(1)); 
    }
};

/// \brief list of process identifierss
typedef atermpp::term_list<process_identifier> process_identifier_list;

/// \brief vector of process_identifiers
typedef std::vector<process_identifier>    process_identifier_vector;

/// \brief Test for a process_identifier expression
/// \param t A term
/// \return True if it is a process_identifier expression
inline
bool is_process_identifier(const atermpp::aterm_appl& t)
{
  return core::detail::gsIsProcVarId(t);
}

// template function overloads
std::string pp(const process_identifier& x);
std::string pp(const process_identifier_list& x);
std::string pp(const process_identifier_vector& x);
void normalize_sorts(process_identifier_vector& x, const data::data_specification& dataspec);

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_IDENTIFIER_H
