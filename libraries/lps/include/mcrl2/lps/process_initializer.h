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

#include <iterator>
#include <cassert>
#include <string>
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/atermpp/convert.h"
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

/// \brief Initial state of a linear process.
// LinearProcessInit(<DataVarId>*, <DataVarIdInit>*)
class process_initializer: public atermpp::aterm_appl
{
  protected:
    /// \brief The assignments of the initializer
    data::assignment_list m_assignments;

  public:
    /// \brief Constructor.
    process_initializer()
      : atermpp::aterm_appl(mcrl2::core::detail::constructLinearProcessInit())
    {}

    /// \brief Constructor.
    process_initializer(const data::assignment_list& assignments)
      : atermpp::aterm_appl(core::detail::gsMakeLinearProcessInit(assignments)),
        m_assignments(assignments)
    {
    }

    /// \brief Constructor.
    /// \param t A term
    process_initializer(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_term_LinearProcessInit(m_term));
      atermpp::aterm_appl::iterator i = t.begin();
      m_assignments = data::assignment_list(*i);
    }

    /// \return The sequence of assignments.
    data::assignment_list assignments() const
    {
      return m_assignments;
    }

    /// \brief Returns the initial state of the LPS.
    /// \param process_parameters The parameters of the correponding linear process
    /// \return The initial state of the LPS.
    data::data_expression_list state(const data::variable_list& process_parameters) const
    {
      return data::replace_free_variables(atermpp::convert<data::data_expression_list>(process_parameters), data::assignment_sequence_substitution(m_assignments));
    }
};

// template function overloads
std::string pp(const process_initializer& x);
std::set<data::variable> find_free_variables(const lps::process_initializer& x);

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PROCESS_INITIALIZER_H
