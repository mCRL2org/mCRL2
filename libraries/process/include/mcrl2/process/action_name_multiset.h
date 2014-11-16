// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/action_name_multiset.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ACTION_NAME_MULTISET_H
#define MCRL2_PROCESS_ACTION_NAME_MULTISET_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2
{

namespace process
{

//--- start generated class action_name_multiset ---//
/// \brief A multiset of action names
class action_name_multiset: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    action_name_multiset()
      : atermpp::aterm_appl(core::detail::default_values::MultActName)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit action_name_multiset(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_MultActName(*this));
    }

    /// \brief Constructor.
    action_name_multiset(const core::identifier_string_list& names)
      : atermpp::aterm_appl(core::detail::function_symbol_MultActName(), names)
    {}

    const core::identifier_string_list& names() const
    {
      return atermpp::down_cast<core::identifier_string_list>((*this)[0]);
    }
};

/// \brief list of action_name_multisets
typedef atermpp::term_list<action_name_multiset> action_name_multiset_list;

/// \brief vector of action_name_multisets
typedef std::vector<action_name_multiset>    action_name_multiset_vector;

/// \brief Test for a action_name_multiset expression
/// \param x A term
/// \return True if \a x is a action_name_multiset expression
inline
bool is_action_name_multiset(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::MultActName;
}

// prototype declaration
std::string pp(const action_name_multiset& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const action_name_multiset& x)
{
  return out << process::pp(x);
}

/// \brief swap overload
inline void swap(action_name_multiset& t1, action_name_multiset& t2)
{
  t1.swap(t2);
}
//--- end generated class action_name_multiset ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ACTION_NAME_MULTISET_H
