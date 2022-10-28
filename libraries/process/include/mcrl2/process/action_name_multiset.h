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

#include "mcrl2/data/data_specification.h"

namespace mcrl2
{

namespace process
{

//--- start generated class action_name_multiset ---//
/// \\brief A multiset of action names
class action_name_multiset: public atermpp::aterm_appl
{
  public:
    /// \\brief Default constructor.
    action_name_multiset()
      : atermpp::aterm_appl(core::detail::default_values::MultActName)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit action_name_multiset(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_MultActName(*this));
    }

    /// \\brief Constructor.
    explicit action_name_multiset(const core::identifier_string_list& names)
      : atermpp::aterm_appl(core::detail::function_symbol_MultActName(), names)
    {}

    /// Move semantics
    action_name_multiset(const action_name_multiset&) noexcept = default;
    action_name_multiset(action_name_multiset&&) noexcept = default;
    action_name_multiset& operator=(const action_name_multiset&) noexcept = default;
    action_name_multiset& operator=(action_name_multiset&&) noexcept = default;

    const core::identifier_string_list& names() const
    {
      return atermpp::down_cast<core::identifier_string_list>((*this)[0]);
    }
};

/// \\brief Make_action_name_multiset constructs a new term into a given address.
/// \\ \param t The reference into which the new action_name_multiset is constructed. 
template <class... ARGUMENTS>
inline void make_action_name_multiset(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_MultActName(), args...);
}

/// \\brief list of action_name_multisets
typedef atermpp::term_list<action_name_multiset> action_name_multiset_list;

/// \\brief vector of action_name_multisets
typedef std::vector<action_name_multiset>    action_name_multiset_vector;

/// \\brief Test for a action_name_multiset expression
/// \\param x A term
/// \\return True if \\a x is a action_name_multiset expression
inline
bool is_action_name_multiset(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::MultActName;
}

// prototype declaration
std::string pp(const action_name_multiset& x);

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
inline void swap(action_name_multiset& t1, action_name_multiset& t2)
{
  t1.swap(t2);
}
//--- end generated class action_name_multiset ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ACTION_NAME_MULTISET_H
