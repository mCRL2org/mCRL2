// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/action_name_multiset.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ACTION_NAME_MULTISET_H
#define MCRL2_PROCESS_ACTION_NAME_MULTISET_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/process/action_label.h"

namespace mcrl2::process
{

//--- start generated class action_name_multiset ---//
/// \\brief A multiset of action names
class action_name_multiset: public atermpp::aterm
{
  public:


    /// Move semantics
    action_name_multiset(const action_name_multiset&) noexcept = default;
    action_name_multiset(action_name_multiset&&) noexcept = default;
    action_name_multiset& operator=(const action_name_multiset&) noexcept = default;
    action_name_multiset& operator=(action_name_multiset&&) noexcept = default;

    const core::identifier_string_list& names() const
    {
      return atermpp::down_cast<core::identifier_string_list>((*this)[0]);
    }
//--- start user section action_name_multiset ---//
    /// \brief Default constructor.
    action_name_multiset()
      : atermpp::aterm(core::detail::default_values::MultActName)
    {}

    /// \brief Constructor from aterm.
    /// \param term A term
    explicit action_name_multiset(const atermpp::aterm& term)
      : atermpp::aterm(term)
    {
      assert(core::detail::check_term_MultActName(*this));
    }

    /// \brief Constructor. Names are sorted lexicographically to establish the sorted-storage invariant.
    explicit action_name_multiset(const core::identifier_string_list& names)
      : atermpp::aterm(core::detail::function_symbol_MultActName(),
                       atermpp::sort_list(names, action_name_compare()))
    {}
//--- end user section action_name_multiset ---//
};

/// \\brief Make_action_name_multiset constructs a new term into a given address.
/// \param[out] t The reference into which the new action_name_multiset is constructed.
template <class... ARGUMENTS>
inline void make_action_name_multiset(atermpp::aterm& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_MultActName(), args...);
  // Re-assign through the sorting constructor to maintain the sorted-storage invariant.
  t = action_name_multiset(atermpp::down_cast<action_name_multiset>(t).names());
}

/// \\brief list of action_name_multisets
using action_name_multiset_list = atermpp::term_list<action_name_multiset>;

/// \\brief vector of action_name_multisets
using action_name_multiset_vector = std::vector<action_name_multiset>;

/// \\brief Test for a action_name_multiset expression
/// \\param x A term
/// \\return True if \\a x is a action_name_multiset expression
inline
bool is_action_name_multiset(const atermpp::aterm& x)
{
  return x.function() == core::detail::function_symbols::MultActName;
}

// prototype declaration
std::string pp(const action_name_multiset& x, bool precedence_aware = true);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const action_name_multiset& x)
{
  return out << process::pp(x);
}

/// \\brief swap overload
inline void swap(action_name_multiset& t1, action_name_multiset& t2) noexcept
{
  t1.swap(t2);
}
//--- end generated class action_name_multiset ---//

} // namespace mcrl2::process



#endif // MCRL2_PROCESS_ACTION_NAME_MULTISET_H
