// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/action_label.h
/// \brief The class action_label.

#ifndef MCRL2_LPS_ACTION_LABEL_H
#define MCRL2_LPS_ACTION_LABEL_H

#include <cassert>
#include <set>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2
{

namespace lps
{

//--- start generated class action_label ---//
/// \brief An action label
class action_label: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    action_label()
      : atermpp::aterm_appl(core::detail::constructActId())
    {}

    /// \brief Constructor.
    /// \param term A term
    action_label(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_ActId(*this));
    }

    /// \brief Constructor.
    action_label(const core::identifier_string& name, const data::sort_expression_list& sorts)
      : atermpp::aterm_appl(core::detail::gsMakeActId(name, sorts))
    {}

    /// \brief Constructor.
    action_label(const std::string& name, const data::sort_expression_list& sorts)
      : atermpp::aterm_appl(core::detail::gsMakeActId(core::identifier_string(name), sorts))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>(atermpp::arg1(*this));
    }

    const data::sort_expression_list& sorts() const
    {
      return atermpp::aterm_cast<const data::sort_expression_list>(atermpp::list_arg2(*this));
    }
};

/// \brief list of action_labels
typedef atermpp::term_list<action_label> action_label_list;

/// \brief vector of action_labels
typedef std::vector<action_label>    action_label_vector;


/// \brief Test for a action_label expression
/// \param t A term
/// \return True if it is a action_label expression
inline
bool is_action_label(const atermpp::aterm_appl& t)
{
  return core::detail::gsIsActId(t);
}

//--- end generated class action_label ---//

// template function overloads
std::string pp(const action_label& x);
std::string pp(const action_label_list& x);
std::string pp(const action_label_vector& x);
action_label_list normalize_sorts(const action_label_list& x, const data::data_specification& dataspec);
std::set<data::sort_expression> find_sort_expressions(const lps::action_label_list& x);

// TODO: These should be removed when the aterm code has been replaced.
std::string pp(const atermpp::aterm& x);
std::string pp(const atermpp::aterm_appl& x);

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_ACTION_LABEL_H
