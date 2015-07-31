// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/untyped_multi_action.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_UNTYPED_MULTI_ACTION_H
#define MCRL2_PROCESS_UNTYPED_MULTI_ACTION_H

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/untyped_data_parameter.h"

namespace mcrl2 {

namespace process {

//--- start generated class untyped_multi_action ---//
/// \brief An untyped multi action or data application
class untyped_multi_action: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    untyped_multi_action()
      : atermpp::aterm_appl(core::detail::default_values::UntypedMultiAction)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit untyped_multi_action(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_UntypedMultiAction(*this));
    }

    /// \brief Constructor.
    untyped_multi_action(const data::untyped_data_parameter_list& actions)
      : atermpp::aterm_appl(core::detail::function_symbol_UntypedMultiAction(), actions)
    {}

    const data::untyped_data_parameter_list& actions() const
    {
      return atermpp::down_cast<data::untyped_data_parameter_list>((*this)[0]);
    }
};

/// \brief list of untyped_multi_actions
typedef atermpp::term_list<untyped_multi_action> untyped_multi_action_list;

/// \brief vector of untyped_multi_actions
typedef std::vector<untyped_multi_action>    untyped_multi_action_vector;

/// \brief Test for a untyped_multi_action expression
/// \param x A term
/// \return True if \a x is a untyped_multi_action expression
inline
bool is_untyped_multi_action(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::UntypedMultiAction;
}

// prototype declaration
std::string pp(const untyped_multi_action& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const untyped_multi_action& x)
{
  return out << process::pp(x);
}

/// \brief swap overload
inline void swap(untyped_multi_action& t1, untyped_multi_action& t2)
{
  t1.swap(t2);
}
//--- end generated class untyped_multi_action ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_UNTYPED_MULTI_ACTION_H
