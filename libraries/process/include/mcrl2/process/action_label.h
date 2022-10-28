// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/action_label.h
/// \brief The class action_label.

#ifndef MCRL2_PROCESS_ACTION_LABEL_H
#define MCRL2_PROCESS_ACTION_LABEL_H

#include "mcrl2/data/data_specification.h"

namespace mcrl2
{

namespace process
{

//--- start generated class action_label ---//
/// \\brief An action label
class action_label: public atermpp::aterm_appl
{
  public:
    /// \\brief Default constructor.
    action_label()
      : atermpp::aterm_appl(core::detail::default_values::ActId)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit action_label(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_ActId(*this));
    }

    /// \\brief Constructor.
    action_label(const core::identifier_string& name, const data::sort_expression_list& sorts)
      : atermpp::aterm_appl(core::detail::function_symbol_ActId(), name, sorts)
    {}

    /// \\brief Constructor.
    action_label(const std::string& name, const data::sort_expression_list& sorts)
      : atermpp::aterm_appl(core::detail::function_symbol_ActId(), core::identifier_string(name), sorts)
    {}

    /// Move semantics
    action_label(const action_label&) noexcept = default;
    action_label(action_label&&) noexcept = default;
    action_label& operator=(const action_label&) noexcept = default;
    action_label& operator=(action_label&&) noexcept = default;

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const data::sort_expression_list& sorts() const
    {
      return atermpp::down_cast<data::sort_expression_list>((*this)[1]);
    }
};

/// \\brief Make_action_label constructs a new term into a given address.
/// \\ \param t The reference into which the new action_label is constructed. 
template <class... ARGUMENTS>
inline void make_action_label(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_ActId(), args...);
}

/// \\brief list of action_labels
typedef atermpp::term_list<action_label> action_label_list;

/// \\brief vector of action_labels
typedef std::vector<action_label>    action_label_vector;

/// \\brief Test for a action_label expression
/// \\param x A term
/// \\return True if \\a x is a action_label expression
inline
bool is_action_label(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::ActId;
}

// prototype declaration
std::string pp(const action_label& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const action_label& x)
{
  return out << process::pp(x);
}

/// \\brief swap overload
inline void swap(action_label& t1, action_label& t2)
{
  t1.swap(t2);
}
//--- end generated class action_label ---//

// template function overloads
std::string pp(const action_label_list& x);
std::string pp(const action_label_vector& x);
action_label_list normalize_sorts(const action_label_list& x, const data::sort_specification& sortspec);
std::set<data::sort_expression> find_sort_expressions(const process::action_label_list& x);

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ACTION_LABEL_H
