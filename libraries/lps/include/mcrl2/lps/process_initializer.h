// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/process_initializer.h
/// \brief The class process_initializer.

#ifndef MCRL2_LPS_PROCESS_INITIALIZER_H
#define MCRL2_LPS_PROCESS_INITIALIZER_H

#include <algorithm>
#include <cassert>
#include <iterator>
#include <string>
#include <utility>
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/detail/assignment_functional.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2
{

namespace lps
{

//--- start generated class process_initializer ---//
/// \brief A process initializer
class process_initializer: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    process_initializer()
      : atermpp::aterm_appl(core::detail::constructLinearProcessInit())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit process_initializer(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_LinearProcessInit(*this));
    }

    /// \brief Constructor.
    process_initializer(const data::assignment_list& assignments)
      : atermpp::aterm_appl(core::detail::gsMakeLinearProcessInit(assignments))
    {}

    const data::assignment_list& assignments() const
    {
      return atermpp::aterm_cast<const data::assignment_list>(atermpp::list_arg1(*this));
    }
//--- start user section process_initializer ---//
    /// \brief Returns the initial state of the LPS.
    /// \param process_parameters The parameters of the correponding linear process
    /// \return The initial state of the LPS.
    data::data_expression_list state(const data::variable_list& process_parameters) const
    {
      return data::replace_variables(atermpp::aterm_cast<data::data_expression_list>(process_parameters), data::assignment_sequence_substitution(assignments()));
    }
//--- end user section process_initializer ---//
};

/// \brief list of process_initializers
typedef atermpp::term_list<process_initializer> process_initializer_list;

/// \brief vector of process_initializers
typedef std::vector<process_initializer>    process_initializer_vector;


/// \brief Test for a process_initializer expression
/// \param x A term
/// \return True if \a x is a process_initializer expression
inline
bool is_process_initializer(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsLinearProcessInit(x);
}

//--- end generated class process_initializer ---//

// template function overloads
std::string pp(const process_initializer& x);
std::set<data::variable> find_free_variables(const lps::process_initializer& x);

} // namespace lps

} // namespace mcrl2

namespace std {
//--- start generated swap functions ---//
template <>
inline void swap(mcrl2::lps::process_initializer& t1, mcrl2::lps::process_initializer& t2)
{
  t1.swap(t2);
}
//--- end generated swap functions ---//
} // namespace std

#endif // MCRL2_LPS_PROCESS_INITIALIZER_H
