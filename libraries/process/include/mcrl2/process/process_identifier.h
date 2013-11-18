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

#ifdef MCRL2_USE_INDEX_TRAITS
#include "mcrl2/data/index_traits.h"
#endif

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
      return atermpp::aterm_cast<const core::identifier_string>((*this)[0]);
    }

    const data::variable_list& variables() const
    {
      return atermpp::aterm_cast<const data::variable_list>((*this)[1]);
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

// prototype declaration
std::string pp(const process_identifier& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const process_identifier& x)
{
  return out << process::pp(x);
}

/// \brief swap overload
inline void swap(process_identifier& t1, process_identifier& t2)
{
  t1.swap(t2);
}

//--- end generated class process_identifier ---//

// template function overloads
std::string pp(const process_identifier_list& x);
std::string pp(const process_identifier_vector& x);
void normalize_sorts(process_identifier_vector& x, const data::data_specification& dataspec);

#ifdef MCRL2_USE_INDEX_TRAITS

typedef std::pair<core::identifier_string, data::variable_list> process_identifier_key_type;

inline
void on_create_process_identifier(const atermpp::aterm& t)
{
  const process_identifier& p = atermpp::aterm_cast<const process_identifier>(t);
  core::index_traits<process_identifier, process_identifier_key_type>::insert(std::make_pair(t.name(), t.variables()));
}

inline
void on_delete_process_identifier(const atermpp::aterm& t)
{
  const process_identifier& p = atermpp::aterm_cast<const process_identifier>(t);
  core::index_traits<process_identifier, process_identifier_key_type>::erase(std::make_pair(t.name(), t.variables()));
}

inline
void register_process_identifier_hooks()
{
  add_creation_hook(core::detail::function_symbol_ProcVarId(), on_create_process_identifier);
  add_deletion_hook(core::detail::function_symbol_ProcVarId(), on_delete_process_identifier);
}

#endif // MCRL2_USE_INDEX_TRAITS

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_IDENTIFIER_H
