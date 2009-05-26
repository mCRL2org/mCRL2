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
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/assignment_functional.h"
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/lps/detail/specification_utility.h"   // compute_initial_state

namespace mcrl2 {

namespace lps {

/// \brief Initial state of a linear process.
// LinearProcessInit(<DataVarId>*, <DataVarIdInit>*)
class process_initializer: public atermpp::aterm_appl
{
  protected:
    /// \brief The free variables of the initializer
    data::variable_list   m_free_variables;

    /// \brief The assignments of the initializer
    data::assignment_list m_assignments;

  public:
    /// \brief Constructor.
    process_initializer()
      : atermpp::aterm_appl(mcrl2::core::detail::constructLinearProcessInit())
    {}

    /// \brief Constructor.
    process_initializer(data::variable_list free_variables,
                        data::assignment_list assignments
                       )
     : atermpp::aterm_appl(core::detail::gsMakeLinearProcessInit(
         free_variables,
         assignments)),
       m_free_variables(free_variables),
       m_assignments(assignments)
    {
    }

    /// \brief Constructor.
    /// \param t A term
    process_initializer(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_term_LinearProcessInit(m_term));
      atermpp::aterm_appl::iterator i   = t.begin();
      m_free_variables = *i++;
      m_assignments = *i;
    }

    /// \brief Returns the sequence of free variables.
    /// \return The sequence of free variables.
    data::variable_list free_variables() const
    {
      return m_free_variables;
    }

    /// \brief Returns the sequence of assignments.
    /// \return The sequence of assignments.
    data::assignment_list assignments() const
    {
      return m_assignments;
    }

    /// \brief Returns the initial state of the LPS.
    /// \return The initial state of the LPS.
    data::data_expression_list state() const
    {
      return detail::compute_initial_state(m_assignments);
    }

    /// \brief Applies a low level substitution function to this term and returns the result.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
    /// \deprecated
    /// \return The substitution result.
    template <typename Substitution>
    process_initializer substitute(Substitution f)
    {
      return process_initializer(f(atermpp::aterm(*this)));
    }

    /// \brief Checks if the process initializer is well typed
    /// \return Returns true if
    /// <ul>
    /// <li>the left hand sides of the data assignments are unique</li>
    /// </ul>
    bool is_well_typed() const
    {
      // check 1)
      if (mcrl2::data::detail::sequence_contains_duplicates(
               boost::make_transform_iterator(m_assignments.begin(), data::detail::assignment_lhs()),
               boost::make_transform_iterator(m_assignments.end()  , data::detail::assignment_lhs())
              )
         )
      {
        std::cerr << "process_initializer::is_well_typed() failed: data assignments " << pp(m_assignments) << " don't have unique left hand sides." << std::endl;
        return false;
      }

      return true;
    }
};

/// \brief Returns the free variables that occur in the process initializer
/// \param init A process initializer 
/// \return The free variables that occur in the process initializer
inline
std::set<data::variable> compute_free_variables(const process_initializer& init)
{
  std::set<data::variable> result;
  for (data::assignment_list::const_iterator i = init.assignments().begin(); i != init.assignments().end(); ++i)
  {
    data::find_all_free_variables(i->rhs(), std::inserter(result, result.end()));
  }
  return result;
}

/// \brief Traverses the process initializer, and writes all sort expressions
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
void traverse_sort_expressions(const process_initializer& init, OutIter dest)
{
  // free variables
  data::traverse_sort_expressions(init.free_variables(), dest);

  // next state
  data::traverse_sort_expressions(init.assignments(), dest);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PROCESS_INITIALIZER_H
