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

//--- start generated class process_identifier ---//
/// \brief A process identifier
class process_identifier: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    process_identifier()
      : atermpp::aterm_appl(core::detail::constructProcVarId())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit process_identifier(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_ProcVarId(*this));
    }

    /// \brief Constructor.
    process_identifier(const core::identifier_string& name, const data::variable_list& variables)
      : atermpp::aterm_appl(core::detail::gsMakeProcVarId(name, variables))
    {}

    /// \brief Constructor.
    process_identifier(const std::string& name, const data::variable_list& variables)
      : atermpp::aterm_appl(core::detail::gsMakeProcVarId(core::identifier_string(name), variables))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>(atermpp::arg1(*this));
    }

    const data::variable_list& variables() const
    {
      return atermpp::aterm_cast<const data::variable_list>(atermpp::list_arg2(*this));
    }
};

/// \brief list of process_identifiers
typedef atermpp::term_list<process_identifier> process_identifier_list;

/// \brief vector of process_identifiers
typedef std::vector<process_identifier>    process_identifier_vector;


/// \brief Test for a process_identifier expression
/// \param x A term
/// \return True if \a x is a process_identifier expression
inline
bool is_process_identifier(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsProcVarId(x);
}

//--- end generated class process_identifier ---//

// template function overloads
std::string pp(const process_identifier& x);
std::string pp(const process_identifier_list& x);
std::string pp(const process_identifier_vector& x);
void normalize_sorts(process_identifier_vector& x, const data::data_specification& dataspec);

} // namespace process

} // namespace mcrl2

namespace std {
//--- start generated swap functions ---//
template <>
inline void swap(mcrl2::process::process_identifier& t1, mcrl2::process::process_identifier& t2)
{
  t1.swap(t2);
}
//--- end generated swap functions ---//
} // namespace std

#endif // MCRL2_PROCESS_PROCESS_IDENTIFIER_H
